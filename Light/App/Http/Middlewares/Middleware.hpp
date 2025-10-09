#pragma once
#include <boost/beast/http.hpp>
#include <functional>

namespace beast = boost::beast;
namespace http = beast::http;

class Middleware {
public:
	using Request = http::request<http::string_body>;
	using Response = http::response<http::string_body>;

	virtual bool handle(Request& req, Response& res) = 0;
	virtual ~Middleware() = default;

	virtual void setCors(const Request& req, Response& res) {
		res.set(http::field::access_control_allow_origin, "http://localhost:3000");
		res.set(http::field::access_control_allow_methods, "POST, GET, OPTIONS, PATCH, PUT, DELETE");
		res.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
		res.set(http::field::access_control_allow_credentials, "true");
		res.result(http::status::ok);
	}
};