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
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            res.result(http::status::bad_request);
            res.body() = "Validation error: Invalid JSON format.";
            res.prepare_payload();
            return;
        } catch (const std::exception& e) {
            std::cerr << "Unexpected error during parsing: " << e.what() << std::endl;
            res.result(http::status::internal_server_error);
            res.body() = "Internal server error.";
            res.prepare_payload();
            return;
        }
        next();
    }
}; 