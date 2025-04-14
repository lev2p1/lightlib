#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
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
#include "Database/Migrations/MigrationManager.hpp"
#include "Database/Migrations/Migrations.hpp"
#include "Database/SQLString.hpp"

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
        Database db;
        //try {
        //    // Создаем таблицу migrations
        //    db.execute(MigrationMigrationsCreate::up());
        //    std::cout << "Table 'migrations' created successfully." << std::endl;
        //}
        //catch (const std::exception& e) {
        //    std::cerr << "Error creating 'migrations' table: " << e.what() << std::endl;
        //    return 1;
        //}

        (new MigrationManager(db))->Initialize();
   
        // Порт
        const unsigned short port = 8080;

        // Контекст для работы с сетью
        net::io_context ioc;

        // Создаем acceptor для прослушивания входящих соединений
        tcp::acceptor acceptor(ioc, { tcp::v4(), port });
        std::cout << "Server is running on port " << port << std::endl;

        // Объявление контроллеров
        auto homeController = std::make_shared<HomeController>();
        auto helloController = std::make_shared<HelloController>();

        Router::get("/users/{id}/{name}", [](const Router::Request& req, Router::Response& res, const std::unordered_map<std::string, std::string>& params) {
            std::string userId = params.at("id");
            std::string userName = params.at("name");
            res.body() = "User ID: " + userId + "\nUser name: " + userName;
            res.result(http::status::ok);
            });

        Router::get("/about", [homeController](const Router::Request& req, Router::Response& res) {
            homeController->about(req, res);
            });

        Router::get("/hello", [helloController](const Router::Request& req, Router::Response& res) {
            helloController->index(req, res);
            });

        Router::get("/user", [helloController](const Router::Request& req, Router::Response& res) {
            helloController->getAttr(req, res);
            });

        Router::post("/hello-store", [helloController](const Router::Request& req, Router::Response& res) {
            helloController->store(req, res);
            });

        Router::post("/login", [helloController](const Router::Request& req, Router::Response& res) {
            helloController->login(req, res);
            });

        Router::get("/test-queue", [helloController](const Router::Request& req, Router::Response& res) {
            helloController->testQueue(req, res);
            });

        Router::get("/test-cache", [helloController](const Router::Request& req, Router::Response& res) {
            helloController->testCache(req, res);
            });

        Router::post("/register", [helloController](const Router::Request& req, Router::Response& res) {
            helloController->reg(req, res);
            });

        while (true) {
            // Ожидаем входящего соединения
            tcp::socket socket(ioc);
            acceptor.accept(socket);

            try {
                // Таймер для тайм-аута
                net::steady_timer timer(ioc);
                timer.expires_after(std::chrono::seconds(30)); // Тайм-аут 30 секунд

                // Асинхронное ожидание тайм-аута
                timer.async_wait([&socket](const boost::system::error_code& ec) {
                    if (!ec) {
                        // Если тайм-аут сработал, закрываем сокет
                        socket.close();
                    }
                    });

                // Буфер для чтения запроса
                beast::flat_buffer buffer;

                // Читаем HTTP-запрос
                http::request<http::string_body> req;
                boost::system::error_code ec;
                http::read(socket, buffer, req, ec);

                if (ec == beast::http::error::end_of_stream) {
                    Logger::log("Client closed the connection prematurely.", "WARNING");
                    continue;
                }
                else if (ec) {
                    Logger::log("Error reading request: " + ec.message(), "ERROR");
                    continue;
                }

                // Отменяем таймер, так как запрос успешно прочитан
                timer.cancel();

                // Создаем HTTP-ответ
                http::response<http::string_body> res;
                res.version(req.version());

                // Обрабатываем запрос с помощью роутера
                Router::handle_request(req, res);

                // Отправляем ответ клиенту
                http::write(socket, res, ec);
                if (ec) {
                    Logger::log("Error sending response: " + ec.message(), "ERROR");
                    continue;
                }

                // Закрываем соединение
                socket.shutdown(tcp::socket::shutdown_send, ec);
                if (ec && ec != boost::system::errc::not_connected) {
                    Logger::log("Error shutting down socket: " + ec.message(), "ERROR");
                }
            }
            catch (const std::exception& e) {
                Logger::log("Error handling request: " + std::string(e.what()), "ERROR");
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        Logger::log(std::string(e.what()), "ERROR");
        return 1;
    }

    Logger::log("Application finished", "INFO");
    return 0;
}