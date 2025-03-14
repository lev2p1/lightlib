#pragma once

#include <boost/beast/http.hpp>
#include "Controller.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

class HomeController : public Controller {
public:

	using Request = http::request<http::string_body>;
	using Response = http::response<http::string_body>;

	void handle(const Request& req, Response& res) override;

	static void about(const Request& req, Response& res);


};
