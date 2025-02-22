#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "../Database.hpp"

// Базовый класс Model с использованием CRTP
template <typename Derived>
class Model {
protected:
    std::map<std::string, std::string> attributes; // Атрибуты модели
    static inline std::vector<std::string> fillable; // Список полей для заполнения
    static inline std::vector<std::string> fields; // Список всех полей

public:
    virtual ~Model() = default;

    // Метод save
    void save() {
        for (const auto& [key, value] : attributes) {
            std::cout << key << ": " << value << std::endl;
        }
    }

    // Устанавливает значение атрибута
    void setAttribute(const std::string& key, const std::string& value) {
        if (isField(key)) {
            this->attributes[key] = value; // Используем this->attributes
        }
        else {
            std::cerr << "Field '" << key << "' is not fillable." << std::endl;
        }
    }

    // Выводит все атрибуты
    void printAttributes() const {
        for (const auto& [key, value] : attributes) {
            std::cout << key << ": " << value << std::endl;
        }
    }

    // Заполняет модель данными
    static std::shared_ptr<Model<Derived>> create(const std::map<std::string, std::string>& data) {
        auto model = std::make_shared<Derived>();
        for (const auto& [key, value] : data) {
            if (Derived::isFillable(key)) {
                model->attributes[key] = value; // Используем неконстантный доступ
                std::cout << key << " = " << value << std::endl;
            }
            else {
                std::cerr << "Field '" << key << "' is not fillable." << std::endl;
                return nullptr;
            }
        }
        return model;
    }

    // Проверяет, является ли поле fillable
    static bool isFillable(const std::string& field) {
        return std::find(Derived::fillable.begin(), Derived::fillable.end(), field) != Derived::fillable.end();
    }

    static bool isField(const std::string& field) {
        return std::find(Derived::fields.begin(), Derived::fields.end(), field) != Derived::fields.end();
    }


    // Возвращает значение атрибута
    std::string getAttribute(const std::string& key) const {
        if (attributes.find(key) != attributes.end()) {
            return attributes.at(key);
        }
        throw std::invalid_argument("Attribute '" + key + "' not found.");
    }

    // Статическая переменная для хранения имени таблицы
    static inline std::string table_name = "default_table";

    // Работа с данными
    static std::shared_ptr<Derived> read(int id) {
        try {
            // Подключение к базе данных
            auto database = std::make_shared<Database>("127.0.0.1", "postgres", "qwerty123", "light");

            // Формируем список полей
            std::string fields_;
            for (size_t i = 0; i < Derived::fields.size(); ++i) {
                fields_ += Derived::fields[i];
                if (i < Derived::fields.size() - 1) {
                    fields_ += ", "; // Добавляем запятую только между полями
                }
            }

            // Формируем SQL-запрос
            std::string sql = "SELECT " + fields_ + " FROM " + Derived::table_name + " WHERE id = " + std::to_string(id);

            // Выполняем запрос
            std::string data = database->query(sql);

            // Проверяем, что данные не пустые
            if (data.empty()) {
                std::cerr << "No data found for id: " << id << std::endl;
                return nullptr;
            }

            // Выводим результат
            std::cout << "Raw data: " << data << std::endl;
            std::cout << "End of data for id: " << id << std::endl;
            std::cout << "Executed SQL: " << sql << std::endl;

            // Создаем объект
            auto user = std::make_shared<Derived>();

            // Разбиваем строку данных на отдельные значения
            std::istringstream iss(data);
            std::string value;
            for (size_t i = 0; i < Derived::fields.size(); ++i) {
                if (std::getline(iss, value, ' ')) {
                    user->setAttribute(Derived::fields[i], value);
                }
                else {
                    std::cerr << "Not enough data for field: " << Derived::fields[i] << std::endl;
                    return nullptr;
                }
            }

            return user;
        }
        catch (const std::exception& e) {
            std::cerr << "Error in read: " << e.what() << std::endl;
            return nullptr;
        }
    }

    static void update(int id, const std::map<std::string, std::string>& data) {
        std::cout << "Updating record in table: " << Derived::table_name << " with id: " << id << std::endl;
        for (const auto& [key, value] : data) {
            std::cout << key << "=" << value << " ";
        }
        std::cout << std::endl;
    }

    static void delete_(int id) {
        std::cout << "Deleting record from table: " << Derived::table_name << " with id: " << id << std::endl;
    }
};