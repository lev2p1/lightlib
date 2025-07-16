#pragma once

#include <memory>
#include "Router.hpp"
#include "../App/Http/Controllers/HomeController.hpp"
#include "../App/Http/Controllers/HelloController.hpp"
#include "../vendor/Debug/Logger.hpp"

typedef const std::unordered_map<std::string, std::string>& Params;

class RouterRegisterer {
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;

public:

	static void init() {
        try {
            auto homeController = std::make_shared<HomeController>();
            auto helloController = std::make_shared<HelloController>();

            Router::get("/users/{id}/{name}", [](const Router::Request& req, Router::Response& res, Params params) {
                std::string userId = params.at("id");
                std::string userName = params.at("name");
                res.body() = "User ID: " + userId + "\nUser name: " + userName;
                res.result(http::status::ok);
                });

            Router::get("/about", [homeController](const Router::Request& req, Router::Response& res) {
                homeController->about(req, res);
                });

            Router::get("/hello", [helloController](const Router::Request& req, Router::Response& res) {
                helloController->index(req, res);
                });

            Router::get("/user", [helloController](const Router::Request& req, Router::Response& res) {
                helloController->getAttr(req, res);
                });

            Router::post("/hello-store", [helloController](const Router::Request& req, Router::Response& res) {
                helloController->store(req, res);
                });

            Router::post("/login", [helloController](const Router::Request& req, Router::Response& res) {
                helloController->login(req, res);
                });

            Router::get("/test-queue", [helloController](const Router::Request& req, Router::Response& res) {
                helloController->testQueue(req, res);
                });

            Router::get("/test-cache", [helloController](const Router::Request& req, Router::Response& res) {
                helloController->testCache(req, res);
                });

            Router::post("/register", [helloController](const Router::Request& req, Router::Response& res) {
                helloController->reg(req, res);
                });

            Router::resourceApi("/customers", homeController);

            Logger::log("Router is registered", "INFO");
        }
        catch (const std::exception& e) {
            Logger::log("Router not registered", "ERROR");
        }
        
	}
};