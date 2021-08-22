﻿#include "crow.h"
#include "middleware.h"
#include "module.h"
using namespace crow;auto d = D_();//auto d = D_pgsql();
					//auto d = D_sqlite("test.db");
int main() { //setlocale(LC_ALL, ".936");
  App</*Middle*/> app;//Global Middleware,and default config
  app.directory("./static").home("i.htm").timeout(2)
	.file_type({"html","ico","css","js","json","svg","png","gif","jpg","txt"});
  //Server rendering and support default route
  app.default_route()([] {
	char name[64];gethostname(name,64);
	json j{{"servername",name}};
	return mustache::load("404NotFound.html").render(j);
  });
  //sql
  app.route("/sql")([] {
	auto q = d.conn();
	//json v = q("select id,name from users_test where id = 1").JSON();
	//std::cout << v;
	int i = 0; q("SELECT 200+2").r__(i);
	std::string s; q(u8"SELECT '你好 世界！'").r__(s);
	return Res(i, s);
  });
  // a request to /path should be forwarded to /path/
  app.route("/path/")([]() {
	return "Trailing slash test case..";
  });
  //json::parse
  app.route("/list")([]() {
	json v = json::parse(R"({"user":{"is":false,"age":25,"weight":50.6,"name":"deaod"},
	  "userList":[{"is":true,"weight":52.0,"age":23,"state":true,"name":"wwzzgg"},
	  {"is":true,"weight":51.0,"name":"best","age":26}]})");
	return v;
  });
  //static reflect
  app.route("/lists")([]() {
	List list = json::parse(R"({"user":{"is":false,"age":25,"weight":50.6,"name":"deaod"},
	  "userList":[{"is":true,"weight":52.0,"age":23,"state":true,"name":"wwzzgg"},
	  {"is":true,"weight":51.0,"name":"best","age":26}]})").get<List>();
	json json_output = json(list);
	return json_output;
  });

  //status code + return json
  app.route("/json")([] {
	json x;
	x["message"] = u8"你好 世界！";
	x["double"] = 3.1415926;
	x["int"] = 2352352;
	x["true"] = true;
	x["false"] = false;
	x["null"] = nullptr;
	x["bignumber"] = 2353464586543265455;
	return Res(203,x);
  });
  //ostringstream
  app.route("/hello/<int>")([](int count) {
	if (count>100) return Res(400);
	std::ostringstream os;
	os<<count<<" bottles of beer!";
	return Res(203,os.str());
  });
  //rank routing
  app.route("/add/<int>/<int>")([](const Req& req,Res& res,int a,int b) {
	std::ostringstream os;
	os<<a+b;
	res.write(os.str());
	res.end();
  });
  // Compile error with message "Handler type is mismatched with URL paramters"
  //CROW_ROUTE(app,"/another/<int>")([](int a, int b){
	  //return response(500);
  //});
  // more json example
  app.route("/add_json").methods("POST"_mt)([](const Req& req) {
	auto x = json::parse(req.body);
	if (!x) return Res(400);
	int sum = x["a"].get<int>()+x["b"].get<int>();
	std::ostringstream os; os<<sum;
	return Res{os.str()};
  });
  app.route("/params")([](const Req& req) {
	std::ostringstream os;
	os<<"Params: "<<req.url_params<<"\n\n";
	os<<"The key 'foo' was "<<(req.url_params.get("foo")==nullptr?"not ":"")<<"found.\n";
	if (req.url_params.get("pew")!=nullptr) {
	  double countD = boost::lexical_cast<double>(req.url_params.get("pew"));
	  os<<"The value of 'pew' is "<<countD<<'\n';
	}
	auto count = req.url_params.get_list("count");
	os<<"The key 'count' contains "<<count.size()<<" value(s).\n";
	for (const auto& countVal:count) os<<" - "<<countVal<<'\n';
	return Res{os.str()};
  });
  //logger::setHandler(std::make_shared<ExampleLogHandler>());
  app.loglevel(LogLevel::WARNING).port(8080).multithreaded().run();
}