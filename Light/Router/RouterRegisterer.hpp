#pragma once

#include "Router.hpp"
#include "../Controllers/HomeController.hpp"
#include "../Controllers/HelloController.hpp"
#include "../vendor/Debug/Logger.hpp"

class RouterRegisterer {

public:

	static void init() {
        try {
            auto homeController = std::make_shared<HomeController>();
            auto helloController = std::make_shared<HelloController>();

            Router::get("/users/{id}/{name}", [](const Router::Request& req, Router::Response& res, const std::unordered_map<std::string, std::string>& params) {
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
            Logger::log("Router is registerer", "INFO");
        }
        catch (const std::exception& e) {
            Logger::log("Router not registerer", "ERROR");
        }
        
	}
};