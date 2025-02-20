#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>
#include <iostream>

// Базовый класс Model с использованием CRTP
template <typename Derived>
class Model {
protected:
    std::map<std::string, std::string> attributes; // Атрибуты модели
    static inline std::vector<std::string> fillable; // Список fillable полей
public:

    // Заполняет модель данными
    static std::shared_ptr<Model> create(const std::map<std::string, std::string>& data) {
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

    // Возвращает значение атрибута
    std::string getAttribute(const std::string& key) const{
        if (attributes.find(key) != attributes.end()) {
            return attributes.at(key);
        }
        throw std::invalid_argument("Attribute '" + key + "' not found.");
    }

    // Устанавливает значение атрибута
    void setAttribute(const std::string& key, const std::string& value) {
        if (isFillable(key)) {
            attributes[key] = value;
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
    virtual ~Model() = default;

    // Статическая переменная для хранения имени таблицы
    static inline std::string table_name = "default_table";

    // Работа с данными
    virtual void load(int id) = 0;
    virtual void save() = 0;
    virtual bool validate() const = 0;

    // Сериализация
    virtual std::string toJson() const = 0;
    virtual void fromJson(const std::string& json) = 0;

    // Метаданные
    virtual std::vector<std::string> getFieldNames() const = 0;
    virtual std::string getFieldType(const std::string& fieldName) const = 0;

    static std::shared_ptr<Derived> read(int id) {
        std::cout << "Reading record from table: " << Derived::table_name << " with id: " << id << std::endl;
        return nullptr; // В реальной реализации возвращаем объект модели
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