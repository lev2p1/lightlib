#pragma once

#include <hiredis/hiredis.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include "../vendor/Debug/Logger.hpp"

class Queue {
public:
    // Подключение к Redis
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

        Logger::log("Succesfuly connect to Redis " + host + ':' + std::to_string(port), "INFO");
    }

    // Отключение от Redis
    static void disconnect() {
        if (context_) {
            redisFree(context_);
            context_ = nullptr;
            std::cout << "Disconnected from Redis." << std::endl;
        }
    }

    // Добавление элемента в очередь
    static void push(const std::string& queue_name, const std::string& value) {
        if (!context_) {
            Logger::log("Not connected to Redis.", "ERROR");
            throw std::runtime_error("Not connected to Redis.");
        }

        redisReply* reply = (redisReply*)redisCommand(context_, "RPUSH %s %s", queue_name.c_str(), value.c_str());
        if (reply == nullptr) {
            Logger::log("Failed to execute Redis command.", "ERROR");
            throw std::runtime_error("Failed to execute Redis command.");
        }

        freeReplyObject(reply);
        std::cout << "Pushed '" << value << "' to queue '" << queue_name << "'." << std::endl;
    }

    // Извлечение элемента из очереди
    static std::string pop(const std::string& queue_name) {
        if (!context_) {
            Logger::log("Not connected to Redis.", "ERROR");
            throw std::runtime_error("Not connected to Redis.");
        }

        redisReply* reply = (redisReply*)redisCommand(context_, "LPOP %s", queue_name.c_str());
        if (reply == nullptr) {
            Logger::log("Failed to execute Redis command.", "ERROR");
            throw std::runtime_error("Failed to execute Redis command.");
        }

        std::string result;
        if (reply->type == REDIS_REPLY_STRING) {
            result = std::string(reply->str, reply->len);
        }
        else if (reply->type == REDIS_REPLY_NIL) {
            result = ""; // Очередь пуста
        }
        else {
            freeReplyObject(reply);
            Logger::log("Unexpected reply type from Redis.", "ERROR");
            throw std::runtime_error("Unexpected reply type from Redis.");
        }

        freeReplyObject(reply);
        std::cout << "Popped '" << result << "' from queue '" << queue_name << "'." << std::endl;
        return result;
    }

    // Получение длины очереди
    static int length(const std::string& queue_name) {
        if (!context_) {
            Logger::log("Not connected to Redis.", "ERROR");
            throw std::runtime_error("Not connected to Redis.");
        }

        redisReply* reply = (redisReply*)redisCommand(context_, "LLEN %s", queue_name.c_str());
        if (reply == nullptr) {
            Logger::log("Failed to execute Redis command.", "ERROR");
            throw std::runtime_error("Failed to execute Redis command.");
        }

        int len = reply->integer;
        freeReplyObject(reply);
        std::cout << "Queue '" << queue_name << "' length: " << len << std::endl;
        return len;
    }

private:
    static redisContext* context_; // Контекст подключения к Redis
};