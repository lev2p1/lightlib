#pragma once

#include <boost/beast/http.hpp>
#include "Controller.hpp"
#include "../../Database/Database.hpp"
#include "../../Database/Models/User.hpp"
#include "../../vendor/Debug/Logger.hpp"
#include <nlohmann/json.hpp>
#include "../../vendor/Facades/Hash.hpp"
#include "../../Service/AuthService.hpp"
#include "../../Database/Queue.hpp"
#include "../../Database/Cache.hpp"
#include "../../Database/SQLBuilder.hpp"
#include "../../Storage/Storage.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

class HelloController : public Controller {
public:

	using Request = http::request<http::string_body>;
	using Response = http::response<http::string_body>;

	void getAttr(const Request& req, Response& res);

	static void index(const Request& req, Response& res);

	static void index_users(const Request& req, Response& res);

	static void store(const Request& req, Response& res);

	static void login(const Request& req, Response& res);

	static void reg(const Request& req, Response& res);

	static void testQueue(const Request& req, Response& res);

	static void testCache(const Request& req, Response& res);

};

using json = nlohmann::json;

void HelloController::index(const Request& req, Response& res)
{
	res.result(http::status::ok);
	res.body() = "Hello from index function";
}

void HelloController::index_users(const Request& req, Response& res) {

}

void HelloController::store(const Request& req, Response& res) {
	try {
		Storage& storage = Storage::getInstance();
		storage.setRootPath("storage/public");

		storage.put("example.txt", "Hello, Storage!");
		std::string content = storage.get("example.txt");
		std::cout << "File content: " << content << std::endl;

		if (storage.exists("example.txt")) {
			std::cout << "File exists!" << std::endl;
		}

		storage.copy("example.txt", "example_copy.txt");
		storage.deleteFile("example.txt");

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

		std::string username = body["username"];
		std::string password = body["password"];


		auto data = User::where("username = '" + username + "'");

		for (const auto& user : data) {
			std::cout << user->attributes["name"];
		}

		std::pair<std::string, std::vector<unsigned char>> hashed_password = Hash::hash(password, Hash::hexStringToBytes(data[0]->attributes["salt"]));
		if (data[0]->getAttribute("password") == hashed_password.first) {
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

		std::string username = body["username"];
		std::string password = body["password"];
		std::string email = body["email"];

		std::pair<std::string, std::vector<unsigned char>> hashed_password = Hash::hash(password);

		User::create({ {"username", username}, {"password", hashed_password.first}, {"email", email}, {"salt", Hash::bytesToHexString(hashed_password.second)}})->save();
		Logger::log("SUCCESS: All right ", "SUCCESS");

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		Logger::log("Error: " + std::string(e.what()), "ERROR");
		Logger::log(req.body(), "INFO");
	}
}

void HelloController::getAttr(const Request& req, Response& res) {

	try {
		setlocale(LC_ALL, "ru");
		User::find(16)->delete_();
		res.result(http::status::ok);
		res.body() = "Data saved";
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
		Cache::set("my_key", "my_value", 10);
		std::string value = Cache::get("my_key");
		Cache::expire("my_key", 20);
		Cache::del("my_key");
		std::cout << value << std::endl;
		res.body() = value;

	}
	catch (const std::exception& e) {
		Logger::log("Error: " + std::string(e.what()), "ERROR");
		std::cerr << "Error: " << e.what() << std::endl;
	}
}
