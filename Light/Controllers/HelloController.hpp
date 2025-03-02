#pragma once

#include <boost/beast/http.hpp>
#include "Controller.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

class HelloController : public Controller {
public:

	using Request = http::request<http::string_body>;
	using Response = http::response<http::string_body>;

	void handle(const Request& req, Response& res);

	void getAttr(const Request& req, Response& res);

	static void index(const Request& req, Response& res);

	static void store(const Request& req, Response& res);

	static void login(const Request& req, Response& res);

};