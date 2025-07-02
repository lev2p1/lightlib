#pragma once

#include <boost/beast/http.hpp>
#include <unordered_map>
#include <memory>
#include <functional>
#include <regex>
#include <iostream>
#include "../App/Http/Controllers/Controller.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

class Router {
public:
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;
    using Handler = std::function<void(const Request&, Response&, const std::unordered_map<std::string, std::string>&)>;
    using SimpleHandler = std::function<void(const Request&, Response&)>;
    using ExtandHandler = std::function<void(const Request&, Response&, std::shared_ptr<Controller>, const std::unordered_map<std::string, std::string>&)>;

 
    static void get(const std::string& path, SimpleHandler handler) {
        static_routes_[http::verb::get][path] = handler;
    }

    static void get(const std::string& path, Handler handler) {
        std::regex pathRegex = convertPathToRegex(path);
        std::vector<std::string> paramNames = extractParamNames(path);
        dynamic_routes_[http::verb::get].emplace_back(pathRegex, RouteInfo{ handler, paramNames });
    }

    static void post(const std::string& path, SimpleHandler handler) {
        static_routes_[http::verb::post][path] = handler;
    }

    static void post(const std::string& path, Handler handler) {
        std::regex pathRegex = convertPathToRegex(path);
        std::vector<std::string> paramNames = extractParamNames(path);
        dynamic_routes_[http::verb::post].emplace_back(pathRegex, RouteInfo{ handler, paramNames });
    }

    static void patch(const std::string& path, SimpleHandler handler) {
        static_routes_[http::verb::patch][path] = handler;
    }

    static void patch(const std::string& path, Handler handler) {
        std::regex pathRegex = convertPathToRegex(path);
        std::vector<std::string> paramNames = extractParamNames(path);
        dynamic_routes_[http::verb::patch].emplace_back(pathRegex, RouteInfo{ handler, paramNames });
    }

    static void put(const std::string& path, SimpleHandler handler) {
        static_routes_[http::verb::put][path] = handler;
    }

    static void put(const std::string& path, Handler handler) {
        std::regex pathRegex = convertPathToRegex(path);
        std::vector<std::string> paramNames = extractParamNames(path);
        dynamic_routes_[http::verb::put].emplace_back(pathRegex, RouteInfo{ handler, paramNames });
    }

    static void delete_(const std::string& path, SimpleHandler handler) {
        static_routes_[http::verb::delete_][path] = handler;
    }

    static void delete_(const std::string& path, Handler handler) {
        std::regex pathRegex = convertPathToRegex(path);
        std::vector<std::string> paramNames = extractParamNames(path);
        dynamic_routes_[http::verb::delete_].emplace_back(pathRegex, RouteInfo{ handler, paramNames });
    }

    static void resourceApi(const std::string& path, std::shared_ptr<Controller> handle) {
        Router::get(path + "/{id}", [handle, path](const Request& req, Response& res, Params params) {
            handle->show(req, res, params);
            });
        Router::post(path, [handle](const Request& req, Response& res) {
            handle->store(req, res);
            });
        Router::put(path + "/{id}/update", [handle](const Request& req, Response& res, Params params) {
            handle->update(req, res, params);
            });
        Router::patch(path + "/{id}/update", [handle](const Request& req, Response& res, Params params) {
            handle->update(req, res, params);
            });
        Router::delete_(path, [handle](const Request& req, Response& res, Params params) {
            handle->delete_(req, res, params);
            });
    }

    static void handle_request(const Request& req, Response& res) {
        auto method = req.method();
        std::string target = req.target();

        if (static_routes_.find(method) != static_routes_.end()) {
            auto& method_routes = static_routes_[method];
            if (method_routes.find(target) != method_routes.end()) {
                method_routes[target](req, res);
                return;
            }
        }

        if (dynamic_routes_.find(method) != dynamic_routes_.end()) {
            for (const auto& route : dynamic_routes_[method]) {
                std::smatch match;
                if (std::regex_match(target, match, route.first)) {
                    std::unordered_map<std::string, std::string> params;
                    for (size_t i = 1; i < match.size(); ++i) {
                        params[route.second.paramNames[i - 1]] = match[i];
                    }
                    route.second.handler(req, res, params);
                    return;
                }
            }
        }

        res.result(http::status::not_found);
        res.body() = "Error 404: Page not found.";
    }

    static void clearRoutes() {
        static_routes_.clear();
        dynamic_routes_.clear();
    }

private:
    struct RouteInfo {
        Handler handler;
        std::vector<std::string> paramNames;
    };

    static std::regex convertPathToRegex(const std::string& path) {
        std::string regexPath = std::regex_replace(path, std::regex("\\{[^/]+\\}"), "([^/]+)");
        return std::regex("^" + regexPath + "$");
    }

    static std::vector<std::string> extractParamNames(const std::string& path) {
        std::vector<std::string> paramNames;
        std::smatch match;
        std::string::const_iterator searchStart(path.cbegin());
        while (std::regex_search(searchStart, path.cend(), match, std::regex("\\{([^/]+)\\}"))) {
            paramNames.push_back(match[1]);
            searchStart = match.suffix().first;
        }
        return paramNames;
    }

    static inline std::unordered_map<http::verb, std::unordered_map<std::string, SimpleHandler>> static_routes_;

    static inline std::unordered_map<http::verb, std::vector<std::pair<std::regex, RouteInfo>>> dynamic_routes_;
};