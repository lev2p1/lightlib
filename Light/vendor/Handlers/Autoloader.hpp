#pragma once

#include <unordered_map>
#include <functional>
#include <string>
#include <any>
#include <memory>

class Autoloader {
public:
    template <typename T>
    void registerClass(const std::string& name, std::function<std::shared_ptr<T>()> factory) {
        factories_[name] = factory;
    }

    template <typename T>
    std::shared_ptr<T> make(const std::string& name) {
        if (factories_.find(name) != factories_.end()) {
            return std::any_cast<std::function<std::shared_ptr<T>()>>(factories_[name])();
        }
        return nullptr;
    }

private:
    std::unordered_map<std::string, std::any> factories_;
};
