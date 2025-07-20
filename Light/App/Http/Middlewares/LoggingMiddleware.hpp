#pragma once
#include "Middleware.hpp"
#include <iostream>
#include <chrono>
#include <ctime>

class LoggingMiddleware : public Middleware {
public:
    void handle(Request& req, Response& res, std::function<void()> next) override {
        auto start = std::chrono::system_clock::now();
        std::cout << "[LOG] Incoming request: " << req.method_string() << " " << req.target() << std::endl;
        next();
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        std::cout << "[LOG] Response status: " << res.result_int() << ", finished at " << std::ctime(&end_time);
    }
}; 