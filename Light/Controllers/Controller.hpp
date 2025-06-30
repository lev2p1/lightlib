#pragma once
#pragma execution_character_set("utf-8")

#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

typedef const std::unordered_map<std::string, std::string>& Params;

class Controller {
public: 

	using Request = http::request<http::string_body>;
	using Response = http::response<http::string_body>;

	inline virtual void show(const Request& req, Response& res, Params params) {}
	inline virtual void store(const Request& req, Response& res){}
	inline virtual void delete_(const Request& req, Response& res, Params params){}
	inline virtual void update(const Request& req, Response& res, Params params){}

	virtual ~Controller() = default;

};
