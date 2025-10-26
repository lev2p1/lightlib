#pragma once

#include <string.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <openssl/thread.h>
#include <openssl/kdf.h>
#include <openssl/rand.h>
#include "../Debug/Logger.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <coroutine>


class Hash {
public:
    static inline uint32_t iterations = 10;
    static inline uint32_t memory_cost = 16;
    static inline uint32_t parallelism = 1;
    static inline uint32_t output_length = 32;

    static std::vector<unsigned char> generateSalt(size_t length) {
        std::vector<unsigned char> salt(length);
        if (RAND_bytes(salt.data(), length) != 1) {
            Logger::log("Failed to generate salt", "ERROR");
            return {};
        }
        return salt;
    }

    static std::pair<std::string, std::vector<unsigned char>>hash(const std::string& password, std::vector<unsigned char> salt = {}) {
        std::vector<unsigned char> output(output_length);
        uint32_t actual_iterations = iterations;

        EVP_KDF* kdf = EVP_KDF_fetch(nullptr, "ARGON2ID", nullptr);
        if (!kdf) {
            Logger::log("Error: Argon2 not supported in OpenSSL", "ERROR");
            return { "", std::vector<unsigned char> {} };
        }

        EVP_KDF_CTX* ctx = EVP_KDF_CTX_new(kdf);
        if (!ctx) {
            Logger::log("Error: Failed to create KDF context", "ERROR");
            EVP_KDF_free(kdf);
            return { "", std::vector<unsigned char> {} };
        }

        salt = salt.size() > 0 ? salt : generateSalt(64);

        OSSL_PARAM params[] = {
            OSSL_PARAM_construct_octet_string("pass", const_cast<char*>(password.data()), password.size()),
            OSSL_PARAM_construct_octet_string("salt", salt.data(), salt.size()),
            OSSL_PARAM_construct_uint("iter", &actual_iterations),
            OSSL_PARAM_construct_uint("memcost", &memory_cost),
            OSSL_PARAM_construct_uint("parallelism", &parallelism),
            OSSL_PARAM_construct_end()
        };

        if (EVP_KDF_derive(ctx, output.data(), output.size(), params) <= 0) {
            Logger::log("Error: Argon2 hashing failed", "ERROR");
            EVP_KDF_CTX_free(ctx);
            EVP_KDF_free(kdf);
            return { "", std::vector<unsigned char> {} };
        }

        std::ostringstream oss;
        for (unsigned char byte : output) {
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
        }

        EVP_KDF_CTX_free(ctx);
        EVP_KDF_free(kdf);

        return { oss.str(),  salt};
    }

    static inline std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
        std::vector<uint8_t> bytes;
        for (size_t i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(strtol(byteString.c_str(), nullptr, 16));
            bytes.push_back(byte);
        }
        return bytes;
    }

    static inline std::string bytesToHexString(const std::vector<uint8_t>& bytes) {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (uint8_t byte : bytes) {
            oss << std::setw(2) << static_cast<int>(byte);
        }
        return oss.str();
    }

    static inline bool verify(const std::string& password, const std::string& stored_hash, const std::vector<unsigned char>& salt) {
        auto [new_hash, _] = Hash::hash(password, salt);

        auto stored_bytes = Hash::hexStringToBytes(stored_hash);
        auto new_bytes = Hash::hexStringToBytes(new_hash);
		Logger::log("Stored hash bytes size: " + std::to_string(stored_bytes.size()), "DEBUG");
		Logger::log("New hash bytes size: " + std::to_string(new_bytes.size()), "DEBUG");   
		Logger::log("Stored hash: " + stored_hash, "DEBUG");
		Logger::log("New hash: " + new_hash, "DEBUG");
        bool match = stored_bytes == new_bytes;

        return match;
    }

    static inline boost::asio::awaitable<std::pair<std::string, std::vector<unsigned char>>> awaitableHash(
    const std::string& password,
    std::vector<unsigned char> salt = {}) {
        co_return Hash::hash(password, salt);
    }

    static inline boost::asio::awaitable<bool> awaitableVerify(
    const std::string& password, const std::string& stored_hash, const std::vector<unsigned char>& salt) {
        co_return Hash::verify(password, stored_hash, salt);
    }
};