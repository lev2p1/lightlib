#pragma once
#include <iostream>
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>
#include "../../../vendor/Handlers/ENV.hpp"
#include "Service.hpp"
#include "../../../Database/Cache.hpp"

class AuthService : public Service{
private:
    using traits = jwt::traits::nlohmann_json;
    using claim_t = jwt::basic_claim<traits>;
    using builder_t = jwt::builder<jwt::default_clock, traits>;

    static inline std::map<std::string, std::string> refreshTokens;

public:
    static inline std::string secret;
    static std::string createAccessToken(const std::string& userId) {
        builder_t token(jwt::default_clock{});

        token.set_issuer("auth0")
            .set_type("JWT")
            .set_payload_claim("userId", jwt::basic_claim<traits>(userId));

        return token.sign(jwt::algorithm::hs256{ secret });
    }

    static std::string createRefreshToken(const std::string& userId) {
        builder_t token(jwt::default_clock{});
        token.set_issuer("auth0")
            .set_type("JWT")
            .set_payload_claim("userId", claim_t(userId));

        std::string refreshToken = token.sign(jwt::algorithm::hs256{ secret });
        Cache::set("refresh:" + userId, refreshToken, 604800);
        return refreshToken;
    }

    static boost::asio::awaitable<bool> deleteRefreshToken_async(const std::string& userId, const std::string& refreshToken) {
        bool valid = co_await AuthService::validateRefreshToken_async(userId, refreshToken);

        if(valid) {
            Cache::del("refresh:" + userId);
            co_return true;
        }

        co_return false;
    }

    static bool validateRefreshToken(const std::string& userId, const std::string& refreshToken) {
        try {
            std::string storedToken = Cache::get("refresh:" + userId);
            if (storedToken.empty() || storedToken != refreshToken) {
                return false;
            }

            auto decoded = jwt::decode<traits>(refreshToken);
            jwt::verify<traits>()
                .allow_algorithm(jwt::algorithm::hs256{ secret })
                .with_issuer("auth0")
                .verify(decoded);

            return decoded.get_payload_claim("userId").as_string() == userId;
        }
        catch (const std::exception&) {
            return false;
        }
    }

    static boost::asio::awaitable<std::string> createRefreshToken_async(const std::string& userId) {
        builder_t token(jwt::default_clock{});
        token.set_issuer("auth0")
             .set_type("JWT")
             .set_payload_claim("userId", claim_t(userId));

        std::string refreshToken = token.sign(jwt::algorithm::hs256{ secret });

        co_await Cache::set_async("refresh:" + userId, refreshToken, 604800);
        co_return refreshToken;
    }

    static boost::asio::awaitable<bool> validateRefreshToken_async(const std::string& userId, const std::string& refreshToken) {
        try {
            std::string storedToken = co_await Cache::get_async("refresh:" + userId);
            if (storedToken.empty() || storedToken != refreshToken) {
                co_return false;
            }

            auto decoded = jwt::decode<traits>(refreshToken);
            jwt::verify<traits>()
                .allow_algorithm(jwt::algorithm::hs256{ secret })
                .with_issuer("auth0")
                .verify(decoded);

            co_return decoded.get_payload_claim("userId").as_string() == userId;
        }
        catch (const std::exception&) {
            co_return false;
        }
    }
};
