#pragma once 
#include <memory>
#include "Router.hpp"
#include <boost/asio/io_context.hpp>
#include "../App/Http/Controllers/UserController.hpp"
#include "../App/Http/Controllers/ResetPasswordController.hpp"
#include "types.hpp"

class RouterRegisterer {
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;
    using Params = std::unordered_map<std::string, std::string>;

public:
    static void init(boost::asio::io_context& io) {
        auto userController = std::make_shared<UserController>();
        auto resetPasswordController = std::make_shared<ResetPasswordController>();

        Router::add(GET, "/index-from-router2", 
            [](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                UserController controller;
                co_await controller.index(req, res);
            });

        Router::add(POST, "/register", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await userController->register_(req, res);
            });

        Router::add(GET, "/verify", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await userController->verify(req, res);
            });

        Router::add(POST, "/login", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await userController->login(req, res);
            });

        Router::add(GET, "/logout", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await userController->logout(req, res);
            });

        Router::add(GET, "/new-profile", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await userController->profile(req, res);
            });

        Router::add(POST, "/login/reset", 
            [resetPasswordController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await resetPasswordController->createToken(req, res);
            });

        Router::add(POST, "/login/by-code", 
            [resetPasswordController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await resetPasswordController->authIfValid(req, res);
            });

        Router::add(POST, "/login/reset-password", 
            [resetPasswordController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                co_await resetPasswordController->resetPassword(req, res);
            });

        Router::add(OPTIONS, "/login", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                userController->setCors(req, res);
                co_return;
            });

        Router::add(OPTIONS, "/verify", 
            [userController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                userController->setCors(req, res);
                co_return;
            });

        Router::add(OPTIONS, "/login/by-code", 
            [resetPasswordController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                resetPasswordController->setCors(req, res);
                co_return;
            });

        Router::add(OPTIONS, "/login/reset", 
            [resetPasswordController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                resetPasswordController->setCors(req, res);
                co_return;
            });

        Router::add(OPTIONS, "/login/reset-password", 
            [resetPasswordController](const Request& req, Response& res, const Params& params) -> boost::asio::awaitable<void> {
                resetPasswordController->setCors(req, res);
                co_return;
            });
    }
};