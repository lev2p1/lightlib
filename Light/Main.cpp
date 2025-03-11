#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <iostream>
#include <string>
#include <memory>
#include "Router/Router.hpp"
#include "Controllers/HomeController.hpp"
#include "Controllers/HelloController.hpp"
#include "Database/Models/User.cpp"
#include "map"
#include "Database/Models/User.cpp"
#include "vendor/Debug/Logger.hpp"
#include "vendor/Handlers/ENV.hpp"
#include "vendor/Facades/Hash.hpp"
#include "Database/Queue.hpp"
#include "Database/Cache.hpp"
#include "Database/Migrations/Initializer.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

// Инициализация статических членов класса
bool ENV::initialized = false;
const std::string ENV::env_file_path = ".env"; // Путь к .env файлу
std::vector<BYTE> Hash::self_salt;
redisContext* Queue::context_ = nullptr;
redisContext* Cache::context_ = nullptr;
std::vector<std::pair<Migration::Handler, bool>> Migration::migrations_;

int main() {

    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    try {
        ENV::initialize();
        
        Hash::self_salt = Hash::hexStringToBytes(ENV::env_variables["APP_KEY"]);

        // Инициализация логгера
        Logger::init("debug.log");

        // Регистрация обработчиков сигналов
        Logger::registerSignalHandlers();

        Queue::connect(ENV::env_variables["REDIS_HOST"], stoi(ENV::env_variables["REDIS_PORT"]));
        Cache::connect(ENV::env_variables["REDIS_HOST"], stoi(ENV::env_variables["REDIS_PORT"]));
        // Логирование сообщений
        Logger::log("Application started", "INFO");

        //Initializer::initMigrations();

        // Порт
        const unsigned short port = 8080;

        // контекст для работы с сетью
        net::io_context ioc;

        // Создаем acceptor для прослушивания входящих соединений
        tcp::acceptor acceptor(ioc, { tcp::v4(), port });
        std::cout << "Server is running on port " << port << std::endl;

        // Объявление контроллеров
        auto homeController = std::make_shared<HomeController>();
        auto helloController = std::make_shared<HelloController>();


        // Маршрут для GET-запроса на главную страницу
        Router::get("/", [homeController](const Router::Request& req, Router::Response& res) {
                homeController.get()->handle(req, res);
            });

        // Маршрут для GET-запроса на страницу "О нас"
        Router::get("/about", [homeController](const Router::Request& req, Router::Response& res) {
            homeController.get()->about(req, res);
            });

        Router::get("/hello", [helloController](const Router::Request& req, Router::Response& res) {
            helloController.get()->index(req, res);
            });

        Router::get("/user", [helloController](const Router::Request& req, Router::Response& res) {
            helloController.get()->getAttr(req, res);
            });

        Router::post("/hello-store", [helloController](const Router::Request& req, Router::Response& res) {
            helloController.get()->store(req, res);
            });

        Router::post("/login", [helloController](const Router::Request& req, Router::Response& res) {
            helloController.get()->login(req, res);
            });

        Router::get("/test-queue", [helloController](const Router::Request& req, Router::Response& res) {
            helloController.get()->testQueue(req, res);
            });

        Router::get("/test-cache", [helloController](const Router::Request& req, Router::Response& res) {
            helloController.get()->testCache(req, res);
            });

        Router::post("/register", [helloController](const Router::Request& req, Router::Response& res) {
            helloController.get()->reg(req, res);
            });

        while (true) {
            // Ожидаем входящего соединения
            tcp::socket socket(ioc);
            acceptor.accept(socket);

            // Буфер для чтения запроса
            beast::flat_buffer buffer;

            // Читаем HTTP-запрос
            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            // Создаем HTTP-ответ
            http::response<http::string_body> res;
            res.version(req.version());
            res.set(http::field::server, "Simple C++ Web Server");

            // Обрабатываем запрос с помощью роутера
            Router::handle_request(req, res);

            // Отправляем ответ клиенту
            http::write(socket, res);

            // Закрываем соединение
            socket.shutdown(tcp::socket::shutdown_send);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        Logger::log("Error: " + std::string(e.what()), "ERROR");
        return 1;
    }

    Logger::log("Application finished", "INFO");
    return 0;
}