#pragma once
#include "Model.hpp"

class User : public Model<User> {
public:
    // Переопределяем статическую переменную
    static inline std::string table_name = "users";
    static inline std::vector<std::string> fillable = {"username", "email", "password"}; // Список fillable полей
    static inline std::vector<std::string> fields = { "id", "username", "email", "password"}; // Список всех полей
    std::map<std::string, std::string> attributes;
};