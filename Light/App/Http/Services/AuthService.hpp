#pragma once
#include <iostream>
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>
#include "../vendor/Handlers/ENV.hpp"
#include "Service.hpp"

class AuthService : public Service{
private:
    using traits = jwt::traits::nlohmann_json;
    using claim_t = jwt::basic_claim<traits>;
    using builder_t = jwt::builder<jwt::default_clock, traits>;

    static inline std::map<std::string, std::string> refreshTokens;
    static inline std::string secret = ENV::env_variables["AUTH_SECRET"];

public:
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
        refreshTokens[userId] = refreshToken;
        return refreshToken;
    }

    static bool validateRefreshToken(const std::string& userId, const std::string& refreshToken) {
        auto it = refreshTokens.find(userId);

        if (it == refreshTokens.end()) return false;

        try {
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
};
