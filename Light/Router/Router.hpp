#pragma once 
#include <iostream>
#include <boost/beast/http.hpp>
#include <unordered_map>
#include <memory>
#include <functional>
#include <regex>
#include "Route.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

class Router {
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;
    using Params = std::unordered_map<std::string, std::string>;
    
    static inline std::unordered_map<http::verb, std::vector<Route>> routes_;

public:
    static void add(http::verb method, const std::string& path, 
                   std::function<boost::asio::awaitable<void>(const Request&, Response&, const Params&)> handler) {
        routes_[method].emplace_back(path, std::move(handler));
    }

    static boost::asio::awaitable<void> handle_request(const Request& req, Response& res) {
        auto method = req.method();
        std::string target = std::string(req.target());
        
        if (routes_.find(method) != routes_.end()) {
            for (const auto& route : routes_[method]) {
                Params params;
                if (route.match(target, params)) {
                    co_await route.process(req, res, params);
                    co_return;
                }
            }
        }
        
        res.result(http::status::not_found);
        res.body() = "404 Not Found";
        res.prepare_payload();
        co_return;
    }
};