#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <boost/beast/http.hpp>
#include <unordered_map>
#include <memory>
#include <functional>
#include "Controller.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

class Router {
public:
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;
    using Handler = std::function<void(const Request&, Response&)>;

    // Добавление маршрута для GET-запросов
    void get(const std::string& path, Handler handler);

    // Добавление маршрута для POST-запросов
    void post(const std::string& path, Handler handler);

    // Обработка входящего запроса
    void handle_request(const Request& req, Response& res);

private:
    // Хранение маршрутов: метод -> путь -> обработчик
    std::unordered_map<http::verb, std::unordered_map<std::string, Handler>> routes_;
};

#endif // ROUTER_HPP