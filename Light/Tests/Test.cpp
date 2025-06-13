#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

#define BOOST_TEST_MODULE RouterTests
#include <boost/test/included/unit_test.hpp>
#include "../Router/Router.hpp"

BOOST_AUTO_TEST_CASE(Router_GetRequest) {
    // ������� ��������� ����� ������
    Router::clearRoutes();

    // ����������� ��������� ��������
    Router::get("/test", [](const Router::Request& req, Router::Response& res) {
        res.result(http::status::ok);
        res.body() = "Test Response";
        res.prepare_payload();
        });

    // �������� ��������� �������
    http::request<http::string_body> req;
    req.method(http::verb::get);
    req.target("/test");

    // �������� ��������� ������
    http::response<http::string_body> res;

    // ��������� �������
    Router::handle_request(req, res);

    // �������� ����������
    BOOST_CHECK(res.result() == http::status::ok);
    BOOST_CHECK(res.body() == "Test Response");
}

BOOST_AUTO_TEST_CASE(Router_PostRequest) {
    // ������� ��������� ����� ������
    Router::clearRoutes();

    // ����������� ��������� ��������
    Router::post("/test", [](const Router::Request& req, Router::Response& res) {
        res.result(http::status::ok);
        res.body() = "Test Post Response";
        res.prepare_payload();
        });

    // �������� ��������� �������
    http::request<http::string_body> req;
    req.method(http::verb::post);
    req.target("/test");

    // �������� ��������� ������
    http::response<http::string_body> res;

    // ��������� �������
    Router::handle_request(req, res);

    // �������� ����������
    BOOST_CHECK(res.result() == http::status::ok);
    BOOST_CHECK(res.body() == "Test Post Response");
}

BOOST_AUTO_TEST_CASE(Router_NotFound) {
    // ������� ��������� ����� ������
    Router::clearRoutes();

    // �������� ��������� �������
    http::request<http::string_body> req;
    req.method(http::verb::get);
    req.target("/nonexistent");

    // �������� ��������� ������
    http::response<http::string_body> res;

    // ��������� �������
    Router::handle_request(req, res);

    // �������� ����������
    BOOST_CHECK(res.result() == http::status::not_found);
    BOOST_CHECK(res.body() == "Error 404: Page not found.");
}