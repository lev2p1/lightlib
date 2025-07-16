#pragma once
#include "Model.hpp"

class Post : public Model<Post> {
public:
    // Переопределяем статическую переменную
    static inline std::string table_name = "posts";
    static inline std::vector<std::string> fillable = { "text", "rating", "description" }; // Список fillable полей
    static inline std::vector<std::string> fields = { "id", "text", "rating", "description" }; // Список всех полей
    std::map<std::string, std::string> attributes;
};