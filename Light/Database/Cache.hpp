#pragma once

#include <hiredis/hiredis.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include "../vendor/Debug/Logger.hpp"

class Cache {
public:
    // ����������� � Redis
    static void connect(const std::string& host = "127.0.0.1", int port = 6379) {
        if (context_) {
            throw std::runtime_error("Already connected to Redis.");
        }

        context_ = redisConnect(host.c_str(), port);
        if (context_ == nullptr || context_->err) {
            if (context_) {
                std::string err = "Connection error: " + std::string(context_->errstr);
                redisFree(context_);
                context_ = nullptr;
                throw std::runtime_error(err);
            }
            else {
                Logger::log("Failed to allocate Redis context.", "ERROR");
                throw std::runtime_error("Failed to allocate Redis context.");
            }
        }

        Logger::log("Successfully connected to Redis " + host + ':' + std::to_string(port), "SUCCESS");
    }

    // ���������� �� Redis
    static void disconnect() {
        if (context_) {
            redisFree(context_);
            context_ = nullptr;
            std::cout << "Disconnected from Redis." << std::endl;
        }
    }

    // ��������� �������� � ���
    static void set(const std::string& key, const std::string& value, int expire_seconds = 0) {
        if (!context_) {
            Logger::log("Not connected to Redis.", "ERROR");
            throw std::runtime_error("Not connected to Redis.");
        }

        redisReply* reply;
        if (expire_seconds > 0) {
            reply = (redisReply*)redisCommand(context_, "SET %s %s EX %d", key.c_str(), value.c_str(), expire_seconds);
        }
        else {
            reply = (redisReply*)redisCommand(context_, "SET %s %s", key.c_str(), value.c_str());
        }

        if (reply == nullptr) {
            Logger::log("Failed to execute Redis command.", "ERROR");
            throw std::runtime_error("Failed to execute Redis command.");
        }

        freeReplyObject(reply);
        std::cout << "Set key '" << key << "' with value '" << value << "'." << std::endl;
    }

    // ��������� �������� �� ����
    static std::string get(const std::string& key) {
        if (!context_) {
            Logger::log("Not connected to Redis.", "ERROR");
            throw std::runtime_error("Not connected to Redis.");
        }

        redisReply* reply = (redisReply*)redisCommand(context_, "GET %s", key.c_str());
        if (reply == nullptr) {
            Logger::log("Failed to execute Redis command.", "ERROR");
            throw std::runtime_error("Failed to execute Redis command.");
        }

        std::string result;
        if (reply->type == REDIS_REPLY_STRING) {
            result = std::string(reply->str, reply->len);
        }
        else if (reply->type == REDIS_REPLY_NIL) {
            result = ""; // ���� �� ������
        }
        else {
            freeReplyObject(reply);
            Logger::log("Unexpected reply type from Redis.", "ERROR");
            throw std::runtime_error("Unexpected reply type from Redis.");
        }

        freeReplyObject(reply);
        std::cout << "Got value '" << result << "' for key '" << key << "'." << std::endl;
        return result;
    }

    // �������� ����� �� ����
    static void del(const std::string& key) {
        if (!context_) {
            Logger::log("Not connected to Redis.", "ERROR");
            throw std::runtime_error("Not connected to Redis.");
        }

        redisReply* reply = (redisReply*)redisCommand(context_, "DEL %s", key.c_str());
        if (reply == nullptr) {
            Logger::log("Failed to execute Redis command.", "ERROR");
            throw std::runtime_error("Failed to execute Redis command.");
        }

        freeReplyObject(reply);
        std::cout << "Deleted key '" << key << "'." << std::endl;
    }

    // ��������� ������� ����� �����
    static void expire(const std::string& key, int expire_seconds) {
        if (!context_) {
            Logger::log("Not connected to Redis.", "ERROR");
            throw std::runtime_error("Not connected to Redis.");
        }

        redisReply* reply = (redisReply*)redisCommand(context_, "EXPIRE %s %d", key.c_str(), expire_seconds);
        if (reply == nullptr) {
            Logger::log("Failed to execute Redis command.", "ERROR");
            throw std::runtime_error("Failed to execute Redis command.");
        }

        freeReplyObject(reply);
        std::cout << "Set expiration for key '" << key << "' to " << expire_seconds << " seconds." << std::endl;
    }

private:
    static redisContext* context_; // �������� ����������� � Redis
};

//// ������������� ������������ ����� ������
//redisContext* Cache::context_ = nullptr;