#include "HelloController.hpp"
#include "../Database/Database.hpp"
#include "../vendor/Debug/Logger.hpp"

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
	try {
		auto db = std::make_shared<Database>("127.0.0.1", "postgres", "qwerty123", "light");

		db->execute("CREATE TABLE IF NOT EXISTS users (id SERIAL PRIMARY KEY, name VARCHAR(255), age INT);");
		db->execute("INSERT INTO users (name, age) VALUES ('Alice', 25);");
		db->execute("INSERT INTO users (name, age) VALUES ('Bob', 30);");
		res.result(http::status::ok);
		res.body() = "Данные отправлены на таблицу Light";

		std::cout << "Operations completed successfully!" << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		Logger::log("Error: " + std::string(e.what()), "ERROR");
	}
	
	//db->~Database();
   
	
}
