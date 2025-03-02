#pragma once
#include "Model.hpp"

class User : public Model<User> {
public:
    // Переопределяем статическую переменную
    static inline std::string table_name = "users";
    static inline std::vector<std::string> fillable = {"name", "age", "password"}; // Список fillable полей
    static inline std::vector<std::string> fields = { "id", "name", "age", "password"}; // Список всех полей
    std::map<std::string, std::string> attributes;
};