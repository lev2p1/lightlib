#pragma once
#include "Middleware.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

class RandomBlockMiddleware : public Middleware {
public:
    void handle(Request& req, Response& res, std::function<void()> next) override {
        if (std::rand() % 2) {
            std::cout << "RandomBlockMiddleware: allowed\n";
            next();
        }
        else {
            std::cout << "RandomBlockMiddleware: blocked\n";
            res.result(http::status::forbidden);
            res.body() = "Blocked by RandomBlockMiddleware";
            res.prepare_payload();
        }
    }
};