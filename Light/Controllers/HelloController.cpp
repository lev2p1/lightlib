#include "HelloController.hpp"
#include "../Database/Database.hpp"
#include "../Database/Models/User.cpp"
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
		auto db = std::make_shared<Database>();

		db->execute("CREATE TABLE IF NOT EXISTS users (id SERIAL PRIMARY KEY, name VARCHAR(255), age INT, password VARCHAR(255));");
		User::create({ {"name", "kirill"}, {"age", "20"}, {"password", "sha-0001"} })->save();
		res.result(http::status::ok);
		res.body() = "Данные отправлены на таблицу Light";
		std::cout << "Operations completed successfully!" << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		Logger::log("Error: " + std::string(e.what()), "ERROR");
	}
	
   
	
}

void HelloController::login(const Request& req, Response& res)
{
	try {
		std::cout << req.body();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		Logger::log("Error: " + std::string(e.what()), "ERROR");
	}
}

void HelloController::getAttr(const Request& req, Response& res) {
	
	try {
		setlocale(LC_ALL, "ru");
		User::delete_(16);
		res.result(http::status::ok);
		res.body() = "Данные сохранены успешно";
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

