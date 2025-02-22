#include "HelloController.hpp"
#include "../Database/Database.hpp"
#include "../Database/Models/User.cpp"

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
	}
	
	//db->~Database();
   
	
}

void HelloController::getAttr(const Request& req, Response& res) {
	try {
	//auto user = User::create({ {"id", "1"}, {"name", "Alice"}, {"email", "alice@example.com"} });
	auto user = User::read(13);
	//std::cout << "Here";
	if (user) {
		//user->save();
		//user->printAttributes();
		std::cout << user->getAttribute("id");
	}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}
