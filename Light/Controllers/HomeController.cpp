#include "HomeController.hpp"

void HomeController::handle(const Request& req, Response& res)
{
	res.result(http::status::ok);
	res.body() = "Welcome to the homepage!";
}



void HomeController::about(const Request& req, Response& res)
{
	res.result(http::status::ok);
	res.body() = "About us: This is a simple C++ web server.";
}
