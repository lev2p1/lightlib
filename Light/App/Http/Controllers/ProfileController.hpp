#pragma once
#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>
#include "Controller.hpp"
#include "../../../vendor/Facades/Hash.hpp"
#include "../../../Database/Models/User.hpp"
#include "../Helpers/Validator.hpp"
#include "../Services/Service.hpp"
#include "../Helpers/Cookie.hpp"
#include "../Services/AuthService.hpp"
#include "../Helpers/Code.hpp"
#include "../../../Database/Models/BackupCode.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

class ProfileController : public Controller {
public:
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;

    boost::asio::awaitable<void> profile(const Request& req, Response& res);
	boost::asio::awaitable<void> update(const Request& req, Response& res);
	boost::asio::awaitable<void> show_backup_codes(const Request& req, Response& res);
	boost::asio::awaitable<void> remove_backup_codes(const Request& req, Response& res);
	boost::asio::awaitable<void> generate_backup_codes(const Request& req, Response& res);
	boost::asio::awaitable<void> enable_2fa(const Request& req, Response& res);
	boost::asio::awaitable<void> disable_2fa(const Request& req, Response& res);
	boost::asio::awaitable<void> verify_2fa_code(const Request& req, Response& res);
	boost::asio::awaitable<void> regenerate_2fa_secret(const Request& req, Response& res);
	boost::asio::awaitable<void> disable_account(const Request& req, Response& res);
	boost::asio::awaitable<void> delete_account(const Request& req, Response& res);
	boost::asio::awaitable<void> change_email(const Request& req, Response& res);
	boost::asio::awaitable<void> change_password(const Request& req, Response& res);
    void setCors(const Request& req, Response& res);

private:
    void setCorsHeaders(Response& res);
};

using json = nlohmann::json;

boost::asio::awaitable<void> ProfileController::profile(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                    co_return;
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        int idStr = std::stoi(id);
        auto user = User::find(idStr);
        if (!user) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        json responseJson = user->toJson();
        res.result(http::status::ok);
        res.body() = responseJson.dump();
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ProfileController::update(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        json body = json::parse(req.body());
        int idStr = std::stoi(id);
        std::map<std::string, std::string> updates;
        if (body.contains("username")) {
            updates["username"] = body["username"];
        }
        if (body.contains("email") && Validator::email(body["email"])) {
            updates["email"] = body["email"];
        }
        if (body.contains("picture")) {
            updates["picture"] = body["picture"];
		}
        if (body.contains("timezone")) {
            updates["timezone"] = body["timezone"];
		}
        if (body.contains("language")) {
            updates["language"] = body["language"];
        }
        if (body.contains("country")) {
            updates["country"] = body["country"];
		}
        User::update(idStr, updates);
        res.result(http::status::ok);
        res.body() = "Profile updated successfully";
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ProfileController::show_backup_codes(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        // Placeholder for actual backup code retrieval logic
        json responseJson = {
            {"backup_codes", {"code1", "code2", "code3"}}
        };
        res.result(http::status::ok);
        res.body() = responseJson.dump();
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ProfileController::remove_backup_codes(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        // Placeholder for actual backup code removal logic
        res.result(http::status::ok);
        res.body() = "Backup codes removed successfully";
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ProfileController::generate_backup_codes(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        
		std::vector<std::string> newCodes = Code::generateMultipleCodes(10, 8, true);
		BackupCode::createBackupCodes(std::stoi(id), newCodes);

        json responseJson = {
            {"backup_codes", newCodes}
        };
        res.result(http::status::ok);
        res.body() = responseJson.dump();
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ProfileController::enable_2fa(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        // Placeholder for actual 2FA enabling logic
        res.result(http::status::ok);
        res.body() = "2FA enabled successfully";
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ProfileController::disable_2fa(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        // Placeholder for actual 2FA disabling logic
        res.result(http::status::ok);
        res.body() = "2FA disabled successfully";
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ProfileController::verify_2fa_code(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        // Placeholder for actual 2FA code verification logic
        res.result(http::status::ok);
        res.body() = "2FA code verified successfully";
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ProfileController::regenerate_2fa_secret(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        // Placeholder for actual 2FA secret regeneration logic
        res.result(http::status::ok);
        res.body() = "2FA secret regenerated successfully";
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ProfileController::disable_account(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        // Placeholder for actual account disabling logic
        res.result(http::status::ok);
        res.body() = "Account disabled successfully";
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ProfileController::delete_account(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        // Placeholder for actual account deletion logic
        res.result(http::status::ok);
        res.body() = "Account deleted successfully";
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ProfileController::change_email(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        // Placeholder for actual email change logic
        res.result(http::status::ok);
        res.body() = "Email changed successfully";
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

boost::asio::awaitable<void> ProfileController::change_password(const Request& req, Response& res) {
    try {
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
                else {
                    res.result(http::status::unauthorized);
                }
            }
        }
        catch (const std::exception& e) {
            res.result(http::status::bad_request);
            res.body() = "Failed to read token";
            co_return;
        }
        bool valid = co_await AuthService::validateRefreshToken_async(id, token);
        if (!valid) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        
        json body = json::parse(req.body());
		int idStr = std::stoi(id);
        auto user = User::find(idStr);
        if (!user) {
            res.result(http::status::unauthorized);
            res.body() = "Unauthorized";
            co_return;
        }
        if (!body.contains("current_password") || !Validator::password(body["current_password"])) {
            res.result(http::status::bad_request);
            res.body() = "Invalid current password format";
            co_return;
        }
        if (!body.contains("new_password") || !Validator::password(body["new_password"])) {
            res.result(http::status::bad_request);
            res.body() = "Invalid password format";
            co_return;
        }
		bool passwordMatch = Hash::verify(body["current_password"], user->getAttribute("password"), Hash::hexStringToBytes(user->getAttribute("salt")));
        if (!passwordMatch) {
            res.result(http::status::bad_request);
            res.body() = "Current password is incorrect";
            co_return;
		}
        if (body["current_password"] == body["new_password"]) {
            res.result(http::status::bad_request);
            res.body() = "New password cannot be the same as the current password";
            co_return;
        }
		auto [hashedPassword, salt] = co_await Hash::awaitableHash(body["new_password"]);
		std::map<std::string, std::string> updates;
		updates["password"] = hashedPassword;
		updates["salt"] = Hash::bytesToHexString(salt);
		User::update(idStr, updates);

        res.result(http::status::ok);
        res.body() = "Password changed successfully";
        co_return;
    }
    catch (std::exception& e) {
        res.result(http::status::bad_request);
        Logger::log(e.what(), "ERROR");
        co_return;
    }
}

void ProfileController::setCors(const Request& req, Response& res) {
    res.set(http::field::access_control_allow_origin, "http://localhost:3000");
    res.set(http::field::access_control_allow_methods, "POST, GET, OPTIONS");
    res.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
    res.set(http::field::access_control_allow_credentials, "true");
    res.result(http::status::ok);
}