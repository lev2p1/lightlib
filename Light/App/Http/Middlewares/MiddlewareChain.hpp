#pragma once
#include "Middleware.hpp"
#include <vector>
#include <memory>
#include <functional>

class MiddlewareChain {
    public:
    using Request = Middleware::Request;
    using Response = Middleware::Response;
    using MiddlewarePtr = std::shared_ptr<Middleware>;

    void runMiddlewares(std::vector<MiddlewarePtr>& middlewares, size_t index, Request& req, Response& res, std::function<void()> controller){
        if (index < middlewares.size()) {
            auto next = [&]() {
                runMiddlewares(middlewares, index + 1, req, res, controller);
            };
            middlewares[index]->handle(req, res, next);
        } else {
            controller();
        }
    }
};