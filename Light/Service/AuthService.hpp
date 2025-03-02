#pragma once 
#include <iostream>
#include <jwt-cpp/jwt.h>
#include "../vendor/Handlers/ENV.hpp"

class AuthService {
private:
    static inline std::map<std::string, std::string> refreshTokens; // Хранилище Refresh Token
    static inline std::string secret = ENV::env_variables["AUTH_SECRET"]; // Секретный ключ

public:
    static std::string createAccessToken(const std::string& userId) {
        return jwt::create()
            .set_issuer("auth0")
            .set_type("JWT")
            .set_payload_claim("userId", jwt::claim(userId))
            .sign(jwt::algorithm::hs256{ secret });
    }

    static std::string createRefreshToken(const std::string& userId) {
        std::string refreshToken = jwt::create()
            .set_issuer("auth0")
            .set_type("JWT")
            .set_payload_claim("userId", jwt::claim(userId))
            .sign(jwt::algorithm::hs256{ secret });

        refreshTokens[userId] = refreshToken;
        return refreshToken;
    }

    static bool validateRefreshToken(const std::string& userId, const std::string& refreshToken) {
        auto it = refreshTokens.find(userId);
        if (it == refreshTokens.end()) return false;

        try {
            auto decoded = jwt::decode(refreshToken);
            jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{ secret })
                .with_issuer("auth0")
                .verify(decoded);

            return decoded.get_payload_claim("userId").as_string() == userId;
        }
        catch (const std::exception& e) {
            return false;
        }
    }
};