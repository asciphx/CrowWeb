#pragma once
#include <chrono>
#include <boost/asio.hpp>
#ifdef ENABLE_SSL
#include <boost/asio/ssl.hpp>
#endif
#include <cstdint>
#include <atomic>
#include <future>
#include <vector>
#include <memory>
#include <filesystem>
//#include <fstream>
#include "cc/http_connection.h"
#include "cc/logging.h"
#include "cc/detail.h"

namespace cc {
  static const char RES_GMT[26] = "%a, %d %b %Y %H:%M:%S GMT";
  using namespace boost;
  using tcp = asio::ip::tcp;
  template <typename Handler, typename Adaptor = SocketAdaptor, typename ... Middlewares>
  class Server {
  public:
	Server(Handler* handler, std::string bindaddr, uint16_t port, std::tuple<Middlewares...>* middlewares = nullptr, uint16_t concurrency = 1, typename Adaptor::Ctx* adaptor_ctx = nullptr)
	  : acceptor_(io_service_, tcp::endpoint(boost::asio::ip::address::from_string(bindaddr), port)),
	  signals_(io_service_, SIGINT, SIGTERM),
	  handler_(handler),
	  concurrency_(concurrency < 2 ? 2 : concurrency),
	  roundrobin_index_(concurrency_ - 1),
	  core_(concurrency_ - 1),
	  port_(port),
	  bindaddr_(std::move(bindaddr)),
	  middlewares_(middlewares),
	  adaptor_ctx_(adaptor_ctx) {}

	void run() {
	  if (!std::filesystem::is_directory(STATIC_DIRECTORY)) {
		std::filesystem::create_directory(STATIC_DIRECTORY);
	  }
	  if (!std::filesystem::is_directory(STATIC_DIRECTORY UPLOAD_DIRECTORY)) {
		std::filesystem::create_directory(STATIC_DIRECTORY UPLOAD_DIRECTORY);
	  }
	  for (int i = 0; i < concurrency_; ++i)
		io_service_pool_.emplace_back(new boost::asio::io_service());
	  get_cached_date_str_pool_.resize(concurrency_);
	  timer_queue_pool_.resize(concurrency_);
	  std::vector<std::future<void>> v;
	  std::atomic<int> init_count(0);
	  for (uint16_t i = 0; i < concurrency_; ++i)
		v.push_back(
		  std::async(std::launch::async, [this, i, &init_count] {
			auto last = std::chrono::steady_clock::now();
			std::string date_str;
			get_cached_date_str_pool_[i] = [&date_str, &last]()->std::string {
			  if (std::chrono::steady_clock::now() - last > std::chrono::seconds(2)) {
				time_t last_time_t = time(0); tm my_tm;
				last = std::chrono::steady_clock::now();
#if defined(_MSC_VER) || defined(__MINGW32__)
				localtime_s(&my_tm, &last_time_t);
#else
				localtime_r(&last_time_t, &my_tm);
#endif
				date_str.resize(0x30);
				date_str.resize(strftime(&date_str[0], 0x2f, RES_GMT, &my_tm));
			  }
			  return date_str;
			};
			// initializing timer queue
			detail::dumb_timer_queue timer_queue(*io_service_pool_[i]);
			timer_queue_pool_[i] = &timer_queue;
			roundrobin_index_[i] = 0;
			++init_count;
			io_service_pool_[i]->run();
			}));

	  LOG_INFO << SERVER_NAME << " server is running at " << bindaddr_ << ":" << acceptor_.local_endpoint().port()
		<< " using " << concurrency_ << " threads";
	  LOG_INFO << "Call `app.loglevel(cc::LogLevel::Warning)` to hide Info level logs.";

	  signals_.async_wait(
		[this](const boost::system::error_code& /*error*/, int /*signal_number*/) {
		  stop();
		});
	  while (concurrency_ != init_count) std::this_thread::yield();
	  do_accept();
	  std::thread([this] {
		io_service_.run();
		LOG_INFO << "Exiting.";
		}).join();
	}
	inline void stop() {
	  io_service_.stop(); for (auto& io_service : io_service_pool_) io_service->stop();
	}
	void signal_clear() { signals_.clear(); }
	void signal_add(int signal_number) { signals_.add(signal_number); }

  private:
	inline uint16_t pick_io_service() {
	  if (roundrobin_index_[0] == 0)return 0;
	  uint16_t i = 0, l = roundrobin_index_[core_];
	  while (++i < core_ && l < roundrobin_index_[i])l = roundrobin_index_[i];
	  return i;
	}
	inline void do_accept() {
	  uint16_t idex = pick_io_service();
	  asio::io_service& is = *io_service_pool_[idex];
	  ++roundrobin_index_[idex];
	  auto p = new Connection<Adaptor, Handler, Middlewares...>(
		is, handler_, middlewares_,
		get_cached_date_str_pool_[idex], *timer_queue_pool_[idex],
		adaptor_ctx_, roundrobin_index_[idex]);
	  acceptor_.async_accept(p->socket(),
		[this, p, &is](const boost::system::error_code& ec) {
		  if (!ec) {
			is.post([p] { p->start(); });
		  } else
			delete p;
		  do_accept();
		});
	}
  private:
	asio::io_service io_service_;
	std::vector<std::unique_ptr<asio::io_service>> io_service_pool_;
	std::vector<detail::dumb_timer_queue*> timer_queue_pool_;//Safe
	std::vector<std::function<std::string()>> get_cached_date_str_pool_;
	tcp::acceptor acceptor_;
	boost::asio::signal_set signals_;

	Handler* handler_;
	uint8_t concurrency_{ 1 };
	uint8_t core_{ 1 };
	uint16_t port_;
	std::string bindaddr_;
	std::vector<std::atomic<uint16_t>> roundrobin_index_;

	std::tuple<Middlewares...>* middlewares_;

#ifdef ENABLE_SSL
	bool use_ssl_{ false };
	boost::asio::ssl::context ssl_context_{ boost::asio::ssl::context::sslv23 };
#endif
	typename Adaptor::Ctx* adaptor_ctx_;
  };
}
