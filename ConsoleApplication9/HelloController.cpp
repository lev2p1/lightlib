#include "HelloController.hpp"

void HelloController::handle(const Request& req, Response& res)
{
	res.result(http::status::ok);
	res.body() = "Hello from handle function";
}

void HelloController::index(const Request& req, Response& res)
{
	res.result(http::status::ok);
	res.body() = "Hello from index function";
}

void HelloController::store(const Request& req, Response& res)
{
	std::string message = req.body();
	res.result(http::status::ok);
	res.body() = std::to_string(message.size()).append(message);
}
