#include "HelloController.hpp"


using json = nlohmann::json;

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
		User::create({ {"name", "kolyan"}, {"age", "20"}, {"password", "sha-0001"} })->save();
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
		if (req.body().empty()) {
			throw std::runtime_error("Request body is empty");
		}

		json body = json::parse(req.body());

		// Извлекаем логин и пароль
		std::string username = body["username"];
		std::string password = body["password"];

		std::string hashed_password = Hash::hash(password, 10);

		auto data = User::where("name = '" + username + "'");

		for (const auto& user : data) {
			user->printAttributes();
			std::cout << user->attributes["name"];
		}

		if (data[0]->getAttribute("password") == hashed_password) {
			auto token = AuthService::createAccessToken(data[0]->getAttribute("id"));
			res.body() = "Authorization: " + token;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		Logger::log("Error: " + std::string(e.what()), "ERROR");
	}
}

void HelloController::reg(const Request& req, Response& res)
{
	try {
		if (req.body().empty()) {
			throw std::runtime_error("Request body is empty");
		}

		json body = json::parse(req.body());

		// Извлекаем логин и пароль
		std::string username = body["username"];
		std::string password = body["password"];
		std::string age = body["age"];

		std::string hashed_password = Hash::hash(password, 10);

		User::create({ {"name", username}, {"age", age}, {"password", hashed_password} })->save();

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

