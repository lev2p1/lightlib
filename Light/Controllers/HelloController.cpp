#include "HelloController.hpp"
#include "../Database/SQLBuilder.hpp"
#include "../Storage/Storage.hpp"


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

void HelloController::store(const Request& req, Response& res) {
	try {
		// Получаем экземпляр Storage
		Storage& storage = Storage::getInstance();

		// Устанавливаем корневую директорию
		storage.setRootPath("storage/public");

		// Сохраняем файл
		storage.put("example.txt", "Hello, Storage!");

		// Читаем файл
		std::string content = storage.get("example.txt");
		std::cout << "File content: " << content << std::endl;

		// Проверяем существование файла
		if (storage.exists("example.txt")) {
			std::cout << "File exists!" << std::endl;
		}

		// Копируем файл
		storage.copy("example.txt", "example_copy.txt");

		// Удаляем файл
		storage.deleteFile("example.txt");

		// Устанавливаем статус ответа
		res.result(http::status::ok);
		res.body() = "Storage operations completed successfully!";
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		Logger::log("Error: " + std::string(e.what()), "ERROR");
		res.result(http::status::internal_server_error);
		res.body() = "Error: " + std::string(e.what());
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

void HelloController::testQueue(const Request& req, Response& res) {
	try {

		Queue::push("kolyan_queue", "Kolyan kolyan");

		std::string popa = Queue::pop("kolyan_queue");
		std::cout << popa << std::endl;
		res.body() = popa;
		
	}
	catch (const std::exception& e) {
		Logger::log("Error: " + std::string(e.what()), "ERROR");
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void HelloController::testCache(const Request& req, Response& res) {
    try {
        Cache::set("my_key", "my_value", 10); // Установка значения с временем жизни 10 секунд
        std::string value = Cache::get("my_key"); // Получение значения
        Cache::expire("my_key", 20); // Установка времени жизни ключа на 20 секунд
        Cache::del("my_key"); // Удаление ключа
        std::cout << value << std::endl;
        res.body() = value;

    }
    catch (const std::exception& e) {
        Logger::log("Error: " + std::string(e.what()), "ERROR");
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


