#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

#define BOOST_TEST_MODULE RouterTests
#include <boost/test/included/unit_test.hpp>
#include "../Router/Router.hpp"

BOOST_AUTO_TEST_CASE(Router_GetRequest) {
    // Очистка маршрутов перед тестом
    Router::clearRoutes();

    // Регистрация тестового маршрута
    Router::get("/test", [](const Router::Request& req, Router::Response& res) {
        res.result(http::status::ok);
        res.body() = "Test Response";
        res.prepare_payload();
        });

    // Создание тестового запроса
    http::request<http::string_body> req;
    req.method(http::verb::get);
    req.target("/test");

    // Создание тестового ответа
    http::response<http::string_body> res;

    // Обработка запроса
    Router::handle_request(req, res);

    // Проверка результата
    BOOST_CHECK(res.result() == http::status::ok);
    BOOST_CHECK(res.body() == "Test Response");
}

BOOST_AUTO_TEST_CASE(Router_PostRequest) {
    // Очистка маршрутов перед тестом
    Router::clearRoutes();

    // Регистрация тестового маршрута
    Router::post("/test", [](const Router::Request& req, Router::Response& res) {
        res.result(http::status::ok);
        res.body() = "Test Post Response";
        res.prepare_payload();
        });

    // Создание тестового запроса
    http::request<http::string_body> req;
    req.method(http::verb::post);
    req.target("/test");

    // Создание тестового ответа
    http::response<http::string_body> res;

    // Обработка запроса
    Router::handle_request(req, res);

    // Проверка результата
    BOOST_CHECK(res.result() == http::status::ok);
    BOOST_CHECK(res.body() == "Test Post Response");
}

BOOST_AUTO_TEST_CASE(Router_NotFound) {
    // Очистка маршрутов перед тестом
    Router::clearRoutes();

    // Создание тестового запроса
    http::request<http::string_body> req;
    req.method(http::verb::get);
    req.target("/nonexistent");

    // Создание тестового ответа
    http::response<http::string_body> res;

    // Обработка запроса
    Router::handle_request(req, res);

    // Проверка результата
    BOOST_CHECK(res.result() == http::status::not_found);
    BOOST_CHECK(res.body() == "Error 404: Page not found.");
}