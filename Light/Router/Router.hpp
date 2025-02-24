#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <boost/beast/http.hpp>
#include <unordered_map>
#include <memory>
#include <functional>
#include "../Controllers/Controller.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

class Router {
public:
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;
    using Handler = std::function<void(const Request&, Response&)>;

    // Добавление маршрута для GET-запросов
    static void get(const std::string& path, Handler handler) {
        routes_[http::verb::get][path] = handler;
    }

    // Добавление маршрута для POST-запросов
    static void post(const std::string& path, Handler handler) {
        routes_[http::verb::post][path] = handler;
    }

    // Добавление маршрута для PUT-запросов
    static void put(const std::string& path, Handler handler) {
        routes_[http::verb::put][path] = handler;
    }

    // Добавление маршрута для PATCH-запросов
    static void patch(const std::string& path, Handler handler) {
        routes_[http::verb::patch][path] = handler;
    }

    // Добавление маршрута для DELETE-запросов
    static void delete_(const std::string& path, Handler handler) {
        routes_[http::verb::delete_][path] = handler;
    }

    // Обработка входящего запроса
    static void handle_request(const Request& req, Response& res) {
        auto method = req.method();
        auto target = req.target();

        // Ищем обработчик для данного метода и пути
        if (routes_.find(method) != routes_.end()) {
            auto& method_routes = routes_[method];
            if (method_routes.find(target) != method_routes.end()) {
                method_routes[target](req, res);
                return;
            }
        }

        // Если маршрут не найден, возвращаем 404
        res.result(http::status::not_found);
        res.body() = "Error 404: Page not found.";
    }

private:
    // Хранение маршрутов: метод -> путь -> обработчик
    static std::unordered_map<http::verb, std::unordered_map<std::string, Handler>> routes_;
};

#endif // ROUTER_HPP