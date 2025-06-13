#pragma once

#include <boost/beast/http.hpp>
#include "Controller.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

class HomeController : public Controller {
public:

    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;

    static void about(const Request& req, Response& res);
};

inline void HomeController::about(const Request& req, Response& res)
{
    res.result(http::status::ok);
    res.body() = "About us: This is a simple C++ web server.";
    res.prepare_payload(); // ������������� ����� ���� ������
}