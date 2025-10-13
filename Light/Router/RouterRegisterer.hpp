#pragma once 
#include <memory>
#include "Router.hpp"
#include <boost/asio/io_context.hpp>
#include "../App/Http/Controllers/UserController.hpp"
#include "../App/Http/Controllers/ResetPasswordController.hpp"

class RouterRegisterer {
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;
    using Params = std::unordered_map<std::string, std::string>;

public:
    static void init(boost::asio::io_context& io) {
        auto userController = std::make_shared<UserController>();
        auto resetPasswordController = std::make_shared<ResetPasswordController>();

        Router::add(http::verb::get, "/index-from-router2", 
            [](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                UserController controller;
                co_await controller.index(req, res);
            });

        Router::add(http::verb::post, "/register", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await userController->register_(req, res);
            });

        Router::add(http::verb::get, "/verify", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await userController->verify(req, res);
            });

        Router::add(http::verb::post, "/login", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await userController->login(req, res);
            });

        Router::add(http::verb::get, "/logout", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await userController->logout(req, res);
            });

        Router::add(http::verb::get, "/new-profile", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await userController->profile(req, res);
            });

        Router::add(http::verb::post, "/login/reset", 
            [resetPasswordController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await resetPasswordController->createToken(req, res);
            });

        Router::add(http::verb::post, "/login/by-code", 
            [resetPasswordController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await resetPasswordController->authIfValid(req, res);
            });

        Router::add(http::verb::post, "/login/reset-password", 
            [resetPasswordController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await resetPasswordController->resetPassword(req, res);
            });

        Router::add(http::verb::options, "/login", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                userController->setCors(req, res);
                co_return;
            });

        Router::add(http::verb::options, "/verify", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                userController->setCors(req, res);
                co_return;
            });

        Router::add(http::verb::options, "/login/by-code", 
            [resetPasswordController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                resetPasswordController->setCors(req, res);
                co_return;
            });

        Router::add(http::verb::options, "/login/reset", 
            [resetPasswordController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                resetPasswordController->setCors(req, res);
                co_return;
            });

        Router::add(http::verb::options, "/login/reset-password", 
            [resetPasswordController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                resetPasswordController->setCors(req, res);
                co_return;
            });
    }
};