#pragma once

#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>
#include "Controller.hpp"
#include "../../../vendor/Facades/Hash.hpp"
#include "../../../Database/Models/User.hpp"
#include "../Helpers/Validator.hpp"
#include "../Services/Service.hpp"
#include "../Helpers/Cookie.hpp"
#include "../../../Database/Cache.hpp"
#include <random>

namespace beast = boost::beast;
namespace http = beast::http;

class ResetPasswordController : public Controller{
public:
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;

    boost::asio::awaitable<void> createToken(const Request& req, Response& res);
    boost::asio::awaitable<void> authIfValid(const Request& req, Response& res);
    void setCors(const Request& req, Response& res);
    
private:
    void setCorsHeaders(Response& res);
};

using json = nlohmann::json;

boost::asio::awaitable<void> ResetPasswordController::createToken(const Request& req, Response& res){
    try{
        json body = json::parse(req.body());
    
        if (!body.contains("email") || !Validator::email(body["email"])) {
            setCorsHeaders(res);
            res.result(http::status::bad_request);
            co_return;
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(100000, 999999);
        
        int code = dis(gen);
    
        std::string code_str = std::to_string(code);

        Cache::set("reset_email_" + std::string(body["email"]), code_str, 360);
        Logger::log("reset_email_" + std::string(body["email"]) + " code: " + code_str, "INFO");

        setCorsHeaders(res);
        res.result(http::status::accepted);

        co_return;
    }
    catch(std::exception& e){
        res.result(http::status::bad_request); 
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ResetPasswordController::authIfValid(const Request& req, Response& res){
    try{
        json body = json::parse(req.body());
    
        if (!body.contains("email") || !body.contains("code") || !Validator::email(body["email"])) {
            res.result(http::status::bad_request);
            setCorsHeaders(res);
            co_return;
        }

        std::string key = "reset_email_" + std::string(body["email"]);
        std::string code = std::string(body["code"]);
        std::string code_from_cache = co_await Cache::get_async(key);
        
        if(code != code_from_cache){
            setCorsHeaders(res);
            res.result(http::status::bad_request);
            co_return;
        }else{
            auto user = User::findByEmail(std::string(body["email"]));

            if(user == nullptr){
                res.result(http::status::unauthorized);
                co_return;
            }

            std::string token = co_await AuthService::createRefreshToken_async(user->getAttribute("id")); 
            res.result(http::status::accepted);
            std::map<std::string, std::string> cookies = {
                { "id", user->getAttribute("id") },
                { "token", token }
            };  

            setCorsHeaders(res);
            Cookie::set(res, cookies);
            co_return;
        }

    }catch(std::exception& e){
        res.result(http::status::bad_request); 
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

void ResetPasswordController::setCorsHeaders(Response& res) {
    res.set(http::field::access_control_allow_origin, "http://localhost:3000");
    res.set(http::field::access_control_allow_methods, "GET, POST, PUT, DELETE, OPTIONS");
    res.set(http::field::access_control_allow_headers, "Content-Type, Authorization, X-Requested-With");
    res.set(http::field::access_control_allow_credentials, "true");
}

void ResetPasswordController::setCors(const Request& req, Response& res) {
    res.set(http::field::access_control_allow_credentials, "true");
    res.set(http::field::access_control_allow_origin, "http://localhost:3000");
    res.set(http::field::access_control_allow_methods, "POST, GET, OPTIONS");
    res.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
    res.result(http::status::ok);
}