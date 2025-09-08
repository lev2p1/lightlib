#pragma once

#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>
#include "Controller.hpp"
#include "../../../vendor/Facades/Hash.hpp"
#include "../../../Database/Models/User.hpp"
#include "../Helpers/Validator.hpp"
#include "../Services/Service.hpp"
#include "../Helpers/Cookie.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

class UserController : public Controller{
public:
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;

    boost::asio::awaitable<void> register_(const Request& req, Response& res);
    boost::asio::awaitable<void> login(const Request& req, Response& res);
    boost::asio::awaitable<void> profile(const Request& req, Response& res);
    void setCors(const Request& req, Response& res);
    
private:
    void setCorsHeaders(Response& res);
};

using json = nlohmann::json;

void UserController::setCorsHeaders(Response& res) {
    res.set(http::field::access_control_allow_origin, "*");
    res.set(http::field::access_control_allow_methods, "GET, POST, PUT, DELETE, OPTIONS");
    res.set(http::field::access_control_allow_headers, "Content-Type, Authorization, X-Requested-With");
    res.set(http::field::access_control_allow_credentials, "true");
}

boost::asio::awaitable<void> UserController::register_(const Request& req, Response& res) {
    try {
        json body = json::parse(req.body());

        if (!body.contains("username") || !body.contains("password") || !body.contains("email")) {
            res.result(http::status::bad_request);
            res.body() = "Missing required fields";
            co_return;
        }

        std::string username = body["username"];
        std::string password = body["password"];
        std::string email = body["email"];

        if (!Validator::password(password) || !Validator::email(email)) {
            res.result(http::status::bad_request);
            res.body() = "Invalid password or email";
            co_return;
        }

        auto [hashedPassword, salt] = co_await Hash::awaitableHash(password);
        std::string hexSalt = Hash::bytesToHexString(salt);

        bool success = User::create({
            {"username", username},
            {"password", hashedPassword},
            {"email", email},
            {"salt", hexSalt}
        })->save();

        if(!success){
            res.result(http::status::internal_server_error);
            res.body() = "Failed to save user";
            co_return;
        }

        res.result(http::status::ok);
        res.body() = "All data received";
    }
    catch (const std::exception& e) {
        res.result(http::status::internal_server_error);
        res.body() = "Internal error";
    }
}

boost::asio::awaitable<void> UserController::login(const Request& req, Response& res){
    try{
        json body;

        try {
            body = json::parse(req.body());
        } catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Invalid JSON format";
            co_return;
        }

        if (!body.contains("username") || !body.contains("password")) {
            res.result(http::status::bad_request);
            res.body() = "Missing required fields: username or password";
            co_return;
        }

        if (!body["username"].is_string() || !body["password"].is_string()) {
            res.result(http::status::bad_request);
            res.body() = "Fields must be strings";
            co_return;
        }

        std::string username = body["username"];
        std::string password = body["password"];

        auto user = User::findByUsername(username);

        if(user == nullptr){
            res.result(http::status::not_found);
            res.body() = "User not found";
            co_return;
        }

        std::string hexHashedPassword = user->getAttribute("password");
        std::string hexSalt = user->getAttribute("salt");
        std::vector<uint8_t> salt = Hash::hexStringToBytes(hexSalt);
        
        if(co_await Hash::awaitableVerify(password, hexHashedPassword, salt)){
            std::string token = co_await AuthService::createRefreshToken_async(user->getAttribute("id"));
            res.result(http::status::accepted);
            std::map<std::string, std::string> cookies = {
                { "id", user->getAttribute("id") },
                { "token", token }
            };  
            Cookie::set(res, cookies);
            co_return;
        }

        res.result(http::status::unauthorized);
        res.body() = "Invalid password";
    }
    catch(std::exception &e){
        res.result(http::status::internal_server_error);
    }
}

boost::asio::awaitable<void> UserController::profile(const Request& req, Response& res){
    try{
        std::string token, id;
        try {
            auto cookieHeader = req.base().find(http::field::cookie);
            
            if (cookieHeader != req.base().end()) {
                std::string rawCookies = std::string(cookieHeader->value());
                auto cookies = Cookie::parseCookies(rawCookies);

                if (cookies.contains("token") && cookies.contains("id")) {
                    token = cookies["token"];
                    id = cookies["id"];
                }
                else{
                    res.result(http::status::unauthorized);
                }
            }

        } catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            setCorsHeaders(res);
            co_return;
        }

        bool valid = co_await AuthService::validateRefreshToken_async(id, token);

        if(!valid){
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            setCorsHeaders(res);
            co_return;
        }

        int idStr = std::stoi(id);
        auto user = User::find(idStr);

        json responseJson = {
            { "id", user->getAttribute("id") },
            { "username", user->getAttribute("username") },
            { "email", user->getAttribute("email") }
        };

        res.result(http::status::ok);
        res.body() = responseJson.dump();
        setCorsHeaders(res);
    }
    catch(std::exception &e){
        res.result(http::status::internal_server_error);
        res.body() = "Something went wrong";
        setCorsHeaders(res);
    }
}

void UserController::setCors(const Request& req, Response& res) {
    res.set(http::field::access_control_allow_origin, "http://localhost:3000");
    res.set(http::field::access_control_allow_methods, "POST, GET, OPTIONS");
    res.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
    res.set(http::field::access_control_allow_credentials, "true");
    res.result(http::status::ok);
}