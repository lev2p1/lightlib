#pragma once

#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>
#include "Controller.hpp"
#include "../../../vendor/Facades/Hash.hpp"
#include "../../../Database/Models/User.hpp"
#include "../Helpers/Validator.hpp"
#include "../Services/Service.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

class UserController : public Controller{
public:
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;

    void register_(const Request& req, Response& res);
    void login(const Request& req, Response& res);
    void profile(const Request& req, Response& res);
};

using json = nlohmann::json;

void UserController::register_(const Request &req, Response& res){
    try{
        json body;

        try {
            body = json::parse(req.body());
        } catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Invalid JSON format";
            return;
        }

        if (!body.contains("username") || !body.contains("password") || !body.contains("email")) {
            res.result(http::status::bad_request);
            res.body() = "Missing required fields: username, password, or email";
            return;
        }

        if (!body["username"].is_string() || !body["password"].is_string() || !body["email"].is_string()) {
            res.result(http::status::bad_request);
            res.body() = "Fields must be strings";
            return;
        }

        std::string username = body["username"];
        std::string password = body["password"];
        std::string email = body["email"];

        if(!Validator::password(password) || !Validator::email(email)){
            res.result(http::status::bad_request);
            res.body() = "Password is not validated";
            return;
        }

        auto [hashedPassword, salt] = Hash::hash(password);
        
        std::string hexSalt = Hash::bytesToHexString(salt);

        Logger::log("Salt: " + hexSalt, "DEBUG");

        User::create({
            {"username", username},
            {"password", hashedPassword},
            {"email", email},
            {"salt", hexSalt}
        })->save();

        res.result(http::status::ok);
        res.body() = "All data getted";
    }
    catch(const std::exception &e){
        res.result(http::status::internal_server_error);
    }
   
}

void UserController::login(const Request& req, Response& res){
    try{
        json body;

        try {
            body = json::parse(req.body());
        } catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Invalid JSON format";
            return;
        }

        if (!body.contains("username") || !body.contains("password")) {
            res.result(http::status::bad_request);
            res.body() = "Missing required fields: username or password";
            return;
        }

        if (!body["username"].is_string() || !body["password"].is_string()) {
            res.result(http::status::bad_request);
            res.body() = "Fields must be strings";
            return;
        }

        std::string username = body["username"];
        std::string password = body["password"];

        auto user = User::findByUsername(username);

        if(user == nullptr){
            res.result(http::status::not_found);
            res.body() = "User not found";
            return;
        }

        std::string hexHashedPassword = user->getAttribute("password");
        std::string hexSalt = user->getAttribute("salt");
        std::vector<uint8_t> salt = Hash::hexStringToBytes(hexSalt);
        
        if(Hash::verify(password, hexHashedPassword, salt)){
            std::string token = AuthService::createRefreshToken(user->getAttribute("id")); 
            res.result(http::status::accepted);
            res.body() = token;
            return;
        }

        res.result(http::status::unauthorized);
        res.body() = "Invalid password";
    }
    catch(std::exception &e){
        res.result(http::status::internal_server_error);
    }
}

void UserController::profile(const Request& req, Response& res){
    try{
        json body;

        try {
            body = json::parse(req.body());
        } catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Invalid JSON format";
            return;
        }

        if (!body.contains("token") || !body.contains("id")) {
            res.result(http::status::bad_request);
            res.body() = "Missing required fields: token or id";
            return;
        }

        auto token = body["token"];
        std::string id = body["id"];

        if(!AuthService::validateRefreshToken(id, token)){
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            return;
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
    }
    catch(std::exception &e){
        res.result(http::status::internal_server_error);
        res.body() = "Somthing went wrong";
    }
}