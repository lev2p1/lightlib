#pragma once 
#include <memory>
#include "Router.hpp"
#include <boost/asio/io_context.hpp>
#include "App/Http/Controllers/UserController.hpp"

class RouterRegisterer {
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;
    using Params = std::unordered_map<std::string, std::string>;

public:
    static void init(boost::asio::io_context& io) {
        Router::add(http::verb::get, "/index-from-router2", 
            [](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                UserController controller;
                co_await controller.index(req, res);
            });
    }
};