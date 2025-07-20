#pragma once
#include <boost/beast/http.hpp>
#include <functional>

namespace beast = boost::beast;
namespace http = beast::http;

class Middleware {
public:
	using Request = http::request<http::string_body>;
	using Response = http::response<http::string_body>;

	virtual void handle(Request& req, Response& res, std::function<void()> next) = 0;
	virtual ~Middleware() = default;
};