#pragma once
#pragma execution_character_set("utf-8")

#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

class Controller {
public: 

	using Request = http::request<http::string_body>;
	using Response = http::response<http::string_body>;

	virtual ~Controller() = default;

};
