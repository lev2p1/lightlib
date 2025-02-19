#include "HelloController.hpp"
#include "Database.hpp"

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
	Database db("localhost", "root", NULL, "light");

	std::string message = req.body();

	db.execute("CREATE TABLE IF NOT EXISTS users (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255), age INT);");

	db.execute("INSERT INTO users (name, age) VALUES ('Alice', 25);");
	db.execute("INSERT INTO users (name, age) VALUES ('Bob', 30);");

	res.result(http::status::ok);
	res.body() = "Данные отправлены на таблицу Light";
}
