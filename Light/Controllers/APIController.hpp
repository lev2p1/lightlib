#pragma once
#include "Controller.hpp"

class APIController : public Controller {
public:
	using Request = http::request<http::string_body>;
	using Response = http::response<http::string_body>;

	virtual void show(Request req, Response& res) = 0;
	virtual void update(Request req, Response& res) = 0;
	virtual void delete_(Request req, Response& res) = 0;
	virtual void store(Request req, Response& res) = 0;
};
