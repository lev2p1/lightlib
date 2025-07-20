#pragma once
#include "Middleware.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

class ValidationMiddleware : public Middleware {
public:
    void handle(Request& req, Response& res, std::function<void()> next) override {
        if (req.body().empty()) {
            res.result(http::status::bad_request);
            res.body() = "Validation error: Empty body";
            res.prepare_payload();
            return;
        }
        try {
            auto body = nlohmann::json::parse(req.body());
        } catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = std::string("Validation error: Invalid JSON. ") + e.what();
            res.prepare_payload();
            return;
        }
        next();
    }
}; 