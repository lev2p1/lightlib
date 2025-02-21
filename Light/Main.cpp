#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <mysql_driver.h>
#include "Router/Router.hpp"
#include "Controllers/HomeController.hpp"
#include "Controllers/HelloController.hpp"
#include "Database/Models/User.cpp"
#include "map"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

int main() {
    std::setlocale(LC_ALL, "UTF-8");
    try {
        // Порт
        const unsigned short port = 8080;

        // контекст для работы с сетью
        net::io_context ioc;

        // Создаем acceptor для прослушивания входящих соединений
        tcp::acceptor acceptor(ioc, { tcp::v4(), port });
        std::cout << "Server is running on port " << port << std::endl;

        Router router;

        // Объявление контроллеров
        auto homeController = std::make_shared<HomeController>();
        auto helloController = std::make_shared<HelloController>();

        // Создание пользователя
        auto concreate_user = User::create({ {"id", "john_doe"}, {"name", "john@example.com"}, {"password", "123456"}});
        if (!concreate_user) {
            std::cout << "null";
        }

        // Чтение пользователя
        auto user = User::read(1);
        //std::cout << "User JSON: " << user->toJson() << std::endl;



        // Маршрут для GET-запроса на главную страницу
        router.get("/", [homeController](const Router::Request& req, Router::Response& res) {
                homeController.get()->handle(req, res);
            });

        // Маршрут для GET-запроса на страницу "О нас"
        router.get("/about", [homeController](const Router::Request& req, Router::Response& res) {
            homeController.get()->about(req, res);
            });

        router.get("/hello", [helloController](const Router::Request& req, Router::Response& res) {
            helloController.get()->index(req, res);
            });

        router.post("/hello-store", [helloController](const Router::Request& req, Router::Response& res) {
            helloController.get()->store(req, res);
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
            router.handle_request(req, res);

            // Отправляем ответ клиенту
            http::write(socket, res);

            // Закрываем соединение
            socket.shutdown(tcp::socket::shutdown_send);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}