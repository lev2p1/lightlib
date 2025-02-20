#include "Router.hpp"

// Добавление маршрута для GET-запросов
void Router::get(const std::string& path, Handler handler) {
    routes_[http::verb::get][path] = handler;
}

// Добавление маршрута для POST-запросов
void Router::post(const std::string& path, Handler handler) {
    routes_[http::verb::post][path] = handler;
}

// Добавление маршрута для PUT-запросов
void Router::put(const std::string& path, Handler handler) {
    routes_[http::verb::put][path] = handler;
}

// Добавление маршрута для PATCH-запросов
void Router::patch(const std::string& path, Handler handler) {
    routes_[http::verb::patch][path] = handler;
}

// Добавление маршрута для DELETE-запросов
void Router::delete_(const std::string& path, Handler handler) {
    routes_[http::verb::delete_][path] = handler;
}

// Обработка входящего запроса
void Router::handle_request(const Request& req, Response& res) {
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