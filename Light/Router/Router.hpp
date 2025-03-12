#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <boost/beast/http.hpp>
#include <unordered_map>
#include <memory>
#include <functional>
#include <regex>
#include <iostream>
#include "../Controllers/Controller.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

class Router {
public:
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;
    using Handler = std::function<void(const Request&, Response&, const std::unordered_map<std::string, std::string>&)>;
    using SimpleHandler = std::function<void(const Request&, Response&)>;

    // Добавление статического маршрута для GET-запросов
    static void get(const std::string& path, SimpleHandler handler) {
        static_routes_[http::verb::get][path] = handler;
    }

    // Добавление динамического маршрута для GET-запросов
    static void get(const std::string& path, Handler handler) {
        std::regex pathRegex = convertPathToRegex(path);
        std::vector<std::string> paramNames = extractParamNames(path);
        dynamic_routes_[http::verb::get].emplace_back(pathRegex, RouteInfo{ handler, paramNames });
    }

    // Добавление статического маршрута для POST-запросов patch put delete
    static void post(const std::string& path, SimpleHandler handler) {
        static_routes_[http::verb::post][path] = handler;
    }

    // Добавление динамического маршрута для POST-запросов
    static void post(const std::string& path, Handler handler) {
        std::regex pathRegex = convertPathToRegex(path);
        std::vector<std::string> paramNames = extractParamNames(path);
        dynamic_routes_[http::verb::post].emplace_back(pathRegex, RouteInfo{ handler, paramNames });
    }

    // Добавление статического маршрута для PATCH-запросов put delete
    static void patch(const std::string& path, SimpleHandler handler) {
        static_routes_[http::verb::patch][path] = handler;
    }

    // Добавление динамического маршрута для PATCH-запросов
    static void patch(const std::string& path, Handler handler) {
        std::regex pathRegex = convertPathToRegex(path);
        std::vector<std::string> paramNames = extractParamNames(path);
        dynamic_routes_[http::verb::patch].emplace_back(pathRegex, RouteInfo{ handler, paramNames });
    }

    // Добавление статического маршрута для PUT-запросов
    static void put(const std::string& path, SimpleHandler handler) {
        static_routes_[http::verb::put][path] = handler;
    }

    // Добавление динамического маршрута для PUT-запросов
    static void put(const std::string& path, Handler handler) {
        std::regex pathRegex = convertPathToRegex(path);
        std::vector<std::string> paramNames = extractParamNames(path);
        dynamic_routes_[http::verb::put].emplace_back(pathRegex, RouteInfo{ handler, paramNames });
    }

    // Добавление статического маршрута для DELETE-запросов put delete
    static void delete_(const std::string& path, SimpleHandler handler) {
        static_routes_[http::verb::delete_][path] = handler;
    }

    // Добавление динамического маршрута для DELETE-запросов
    static void delete_(const std::string& path, Handler handler) {
        std::regex pathRegex = convertPathToRegex(path);
        std::vector<std::string> paramNames = extractParamNames(path);
        dynamic_routes_[http::verb::delete_].emplace_back(pathRegex, RouteInfo{ handler, paramNames });
    }

    // Обработка входящего запроса
    static void handle_request(const Request& req, Response& res) {
        auto method = req.method();
        std::string target = req.target();

        // Сначала проверяем статические маршруты
        if (static_routes_.find(method) != static_routes_.end()) {
            auto& method_routes = static_routes_[method];
            if (method_routes.find(target) != method_routes.end()) {
                method_routes[target](req, res);
                return;
            }
        }

        // Если статический маршрут не найден, проверяем динамические маршруты
        if (dynamic_routes_.find(method) != dynamic_routes_.end()) {
            for (const auto& route : dynamic_routes_[method]) {
                std::smatch match;
                if (std::regex_match(target, match, route.first)) {
                    // Извлекаем параметры из пути
                    std::unordered_map<std::string, std::string> params;
                    for (size_t i = 1; i < match.size(); ++i) {
                        params[route.second.paramNames[i - 1]] = match[i];
                    }
                    // Вызываем обработчик
                    route.second.handler(req, res, params);
                    return;
                }
            }
        }

        // Если маршрут не найден, возвращаем 404
        res.result(http::status::not_found);
        res.body() = "Error 404: Page not found.";
    }

    // Очистка всех маршрутов
    static void clearRoutes() {
        static_routes_.clear();
        dynamic_routes_.clear();
    }

private:
    // Структура для хранения информации о динамическом маршруте
    struct RouteInfo {
        Handler handler;
        std::vector<std::string> paramNames;
    };

    // Преобразование пути в регулярное выражение
    static std::regex convertPathToRegex(const std::string& path) {
        std::string regexPath = std::regex_replace(path, std::regex("\\{[^/]+\\}"), "([^/]+)");
        return std::regex("^" + regexPath + "$");
    }

    // Извлечение имен параметров из пути
    static std::vector<std::string> extractParamNames(const std::string& path) {
        std::vector<std::string> paramNames;
        std::smatch match;
        std::string::const_iterator searchStart(path.cbegin());
        while (std::regex_search(searchStart, path.cend(), match, std::regex("\\{([^/]+)\\}"))) {
            paramNames.push_back(match[1]);
            searchStart = match.suffix().first;
        }
        return paramNames;
    }

    // Хранение статических маршрутов: метод -> путь -> обработчик
    static inline std::unordered_map<http::verb, std::unordered_map<std::string, SimpleHandler>> static_routes_;

    // Хранение динамических маршрутов: метод -> вектор пар (регулярное выражение, RouteInfo)
    static inline std::unordered_map<http::verb, std::vector<std::pair<std::regex, RouteInfo>>> dynamic_routes_;
};

#endif // ROUTER_HPP