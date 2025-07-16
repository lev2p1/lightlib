#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "../Database.hpp"
#include "../../vendor/Debug/Logger.hpp"
#include "ModelQueryBuilder.hpp"
#include "../SQLBuilder.hpp"

template <typename Derived>
class Model {
protected:
    static inline std::vector<std::string> fillable; // Список полей для заполнения
    static inline std::vector<std::string> fields; // Список всех полей

    std::map<std::string, std::string> attributes;

    // ��������������� ������� ��� ������������� SQL-��������
    std::string escape(const std::string& value) const {
        std::string escaped = value;
        // ������� ������������� ������� ��� �������
        size_t pos = 0;
        while ((pos = escaped.find("'", pos)) != std::string::npos) {
            escaped.replace(pos, 1, "''");
            pos += 2;
        }
        return escaped;
    }

public:
    virtual ~Model() = default;

    // Новый статический метод для построения запросов через ModelQueryBuilder
    static ModelQueryBuilder<Derived> query() {
        return ModelQueryBuilder<Derived>(Derived::table_name);
    }

    //   
    void setAttribute(const std::string& key, const std::string& value) {
        if (isField(key)) {
            attributes[key] = value;
        }
        else {
            std::cerr << "Field '" << key << "' is not fillable." << std::endl;
        }
    }

    void debugPrintAttributes() const {
        std::cout << "=== MODEL ATTRIBUTES ===\n";
        for (const auto& [key, val] : attributes) {
            std::cout << key << " = '" << val << "'\n";
        }
    }

    // �������� �������� ��������
    std::string getAttribute(const std::string& key) const {
        auto it = attributes.find(key);
        if (it != attributes.end()) {
            return it->second;
        }
        throw std::invalid_argument("Attribute '" + key + "' not found.");
    }

    // ���������� ������
    void save() {
        auto database = std::make_shared<Database>();

        if (attributes.empty()) {
            std::cerr << "Attributes are empty" << std::endl;
            return;
        }

        // Формируем значения для Insert
        std::map<std::string, std::string> insertValues;
        for (const auto& [key, value] : attributes) {
            insertValues[key] = "'" + escape(value) + "'";
        }

        SQLQueryBuilder builder(Derived::table_name);
        builder.Insert(insertValues);
        std::string query = builder.get();

        try {
            database->execute(query);
        }
        catch (const std::exception& e) {
            std::cerr << "Save failed: " << e.what() << std::endl;
        }
    }

    // ����������� ������
    static bool isFillable(const std::string& field) {
        return std::find(fillable.begin(), fillable.end(), field) != fillable.end();
    }

    static bool isField(const std::string& field) {
        return std::find(Derived::fields.begin(), Derived::fields.end(), field) != Derived::fields.end();
    }

    // �������� ������ �� ������
    static std::shared_ptr<Derived> create(const std::map<std::string, std::string>& data) {
        auto model = std::make_shared<Derived>();
        for (const auto& [key, value] : data) {
            if (isField(key)) {
                model->setAttribute(key, value);
            }
        }
        return model;
    }

    //     ID
    static std::shared_ptr<Derived> find(int id) {
        try {
            auto results = query().Where("id = " + std::to_string(id)).Limit(1).get();
            if (results.empty()) {
                Logger::log("No data found by id", "WARNING");
                return nullptr;
            }
            return results.front();
        }
        catch (const std::exception& e) {
            Logger::log("Finding error", "ERROR");
            return nullptr;
        }
    }

    // ���������� ������
    static void update(int id, const std::map<std::string, std::string>& data) {
        try {
            SQLQueryBuilder builder(Derived::table_name);
            std::map<std::string, std::string> updateValues;
            for (const auto& [key, value] : data) {
                if (isField(key)) {
                    updateValues[key] = "'" + value + "'";
                }
            }
            builder.Update(updateValues).Where("id = " + std::to_string(id));
            auto db = std::make_shared<Database>();
            db->execute(builder.get());
        }
        catch (const std::exception& e) {
            std::cerr << "Update failed: " << e.what() << std::endl;
        }
    }


    void delete_() {
        try {
            auto database = std::make_shared<Database>();
            SQLQueryBuilder builder(Derived::table_name);
            builder.Delete().Where("id = " + this->getAttribute("id"));
            std::string query = builder.get();
            database->execute(query);
        }
        catch (const std::exception& e) {
            std::cerr << "Delete failed: " << e.what() << std::endl;
        }
    }

    // ����� �� �������
    static std::vector<std::shared_ptr<Derived>> where(const std::string& condition) {
        return query().Where(condition).get();
    }
};