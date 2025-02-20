#pragma once

#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

class Controller {
public: 

	using Request = http::request<http::string_body>;
	using Response = http::response<http::string_body>;

	virtual void handle(const Request& req, Response& res) = 0;

	virtual ~Controller() = default;

};
