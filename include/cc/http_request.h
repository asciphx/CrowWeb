
#pragma once

#include <boost/asio.hpp>

#include "cc/common.h"
#include "cc/ci_map.h"
#include "cc/query_string.h"

namespace cc {  template<typename T> inline const std::string& get_header_value(const T& headers, const std::string& key) { if (headers.count(key)) { return headers.find(key)->second; } static std::string empty; return empty; }  struct Req { HTTP method; std::string raw_url;   std::string url;   query_string url_params;  ci_map headers; std::string body; std::string remote_ip_address;  bool keep_alive, close_connection, upgrade; void* middleware_context{}; void* middleware_container{}; boost::asio::io_service* io_service{};  Req() : method(HTTP::GET) {}  Req(HTTP method, std::string raw_url, std::string url, query_string url_params, ci_map headers, std::string body, bool has_keep_alive, bool has_close_connection, bool is_upgrade) : method(method), raw_url(std::move(raw_url)), url(std::move(url)), url_params(std::move(url_params)), headers(std::move(headers)), body(std::move(body)), keep_alive(has_keep_alive), close_connection(has_close_connection), upgrade(is_upgrade) {} void add_header(std::string key, std::string value) { headers.emplace(std::move(key), std::move(value)); } const std::string& get_header_value(const std::string& key) const { return cc::get_header_value(headers, key); }  template<typename CompletionHandler> void post(CompletionHandler handler) { io_service->post(handler); }  template<typename CompletionHandler> void dispatch(CompletionHandler handler) { io_service->dispatch(handler); } };} 