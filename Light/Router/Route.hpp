#pragma once 
#include <string>
#include <regex>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <boost/asio/awaitable.hpp>
#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;
using Params = std::unordered_map<std::string, std::string>;

class Route {
    std::regex pathRegex_;
    std::vector<std::string> paramNames_;
    std::string originalPath_;
    std::function<boost::asio::awaitable<void>(const Request&, Response&, const Params&)> handler_;

public:
    Route(const std::string& path, 
          std::function<boost::asio::awaitable<void>(const Request&, Response&, const Params&)> handler)
        : originalPath_(path), handler_(std::move(handler)) {
        pathRegex_ = convertPathToRegex(path);
        paramNames_ = extractParamNames(path);
    }

    bool match(const std::string& target, Params& outParams) const {
        std::smatch match;
        if (std::regex_match(target, match, pathRegex_)) {
            for (size_t i = 0; i < paramNames_.size(); ++i) {
                outParams[paramNames_[i]] = match[i + 1];
            }
            return true;
        }
        return false;
    }

    boost::asio::awaitable<void> process(const Request& req, Response& res, const Params& params) const {
        if (handler_) {
            co_await handler_(req, res, params);
        }
    }

    const std::string& path() const { return originalPath_; }

private:
    static std::regex convertPathToRegex(const std::string& path) {
        std::string regexStr = "^";
        std::istringstream stream(path);
        std::string token;
        
        while (std::getline(stream, token, '/')) {
            if (token.empty()) continue;
            
            if (token[0] == ':') {
                regexStr += "/([^/]+)";
            } else {
                regexStr += "/" + token;
            }
        }
        
        if (regexStr == "^") {
            regexStr += "/?$";
        } else {
            regexStr += "$";
        }
        
        return std::regex(regexStr);
    }

    static std::vector<std::string> extractParamNames(const std::string& path) {
        std::vector<std::string> names;
        std::istringstream stream(path);
        std::string token;
        
        while (std::getline(stream, token, '/')) {
            if (!token.empty() && token[0] == ':') {
                names.push_back(token.substr(1));
            }
        }
        return names;
    }
};