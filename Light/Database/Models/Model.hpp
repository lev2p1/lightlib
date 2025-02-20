#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>
#include <iostream>

// Абстрактный класс Model
class Model {
public:
    virtual ~Model() = default;

    // Атрибуты
    virtual std::string getAttribute(const std::string& key) const = 0;
    virtual void setAttribute(const std::string& key, const std::string& value) = 0;

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

    // Статические CRUD-операции
    static void create(const std::map<std::string, std::string>& data) {
        // Используем статическую переменную table_name
        std::cout << "Creating record in table: " << table_name << std::endl;
        for (const auto& [key, value] : data) {
            std::cout << key << "=" << value << " ";
        }
        std::cout << std::endl;
    }

    static std::shared_ptr<Model> read(int id) {
        std::cout << "Reading record from table: " << table_name << " with id: " << id << std::endl;
        return nullptr; // В реальной реализации возвращаем объект модели
    }

    static void update(int id, const std::map<std::string, std::string>& data) {
        std::cout << "Updating record in table: " << table_name << " with id: " << id << std::endl;
        for (const auto& [key, value] : data) {
            std::cout << key << "=" << value << " ";
        }
        std::cout << std::endl;
    }

    static void delete_(int id) {
        std::cout << "Deleting record from table: " << table_name << " with id: " << id << std::endl;
    }

protected:
    // Статическая переменная для хранения имени таблицы
    static inline std::string table_name = "default_table";
};