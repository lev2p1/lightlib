#pragma once
#include "Middleware.hpp"
#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include "../../Service/AuthService.hpp"

class AuthMiddleware : public Middleware {
public:
	void handle(Request& req, Response& res, std::function<void()> next) override {
		auto authHeader = req[http::field::authorization];
		if (authHeader.empty()) {
			unauthorized(res, "Token is missing");
			return;
		}

		std::string headerStr = std::string(authHeader);
		size_t pos = headerStr.find(' ');
		if (pos == std::string::npos || headerStr.substr(0, pos) != "Bearer") {
			unauthorized(res, "Invalid authorization format");
			return;
		}
		std::string token = headerStr.substr(pos + 1);
		if (token.empty()) {
			unauthorized(res, "Empty token");
			return;
		}

		try {
			auto decoded = jwt::decode(token);
			auto auth_secret_it = ENV::env_variables.find("AUTH_SECRET");
			if (auth_secret_it == ENV::env_variables.end() || auth_secret_it->second.empty()) {
				unauthorized(res, "AUTH_SECRET is missing");
				return;
			}
			std::string auth_secret = auth_secret_it->second;
			jwt::verify()
				.allow_algorithm(jwt::algorithm::hs256{ auth_secret })
				.with_issuer("auth0")
				.verify(decoded);
		} catch (const std::exception& e) {
			unauthorized(res, std::string("JWT verification failed: ") + e.what());
			return;
		}
		next();
	}

	private:
	void unauthorized(Response& res, const std::string& message) {
		nlohmann::json j = { {"error", "Unauthorized"}, {"message", message} };
		res.result(http::status::unauthorized);
		res.set(http::field::content_type, "application/json");
		res.body() = j.dump();
		res.prepare_payload();
	}
};