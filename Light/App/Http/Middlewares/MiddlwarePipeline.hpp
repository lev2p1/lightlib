#pragma once
#include "CorsMiddleware.hpp"

class MiddlewarePipeline {
    std::vector<std::shared_ptr<Middleware>> chain;

public:
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;

    MiddlewarePipeline(std::vector<std::shared_ptr<Middleware>> chain){
        this->chain = chain;
    }

    MiddlewarePipeline() : chain({}) {}

    void use(std::shared_ptr<Middleware> mw) {
        chain.push_back(mw);
    }

    bool run(Request& req, Response& res) {
        for (auto& mw : chain) {
            if(!mw->handle(req, res)) return false;  
        }
        return true;
    }
};