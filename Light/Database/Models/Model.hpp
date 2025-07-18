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
#include "../SQLString.hpp"

template <typename Derived>
class Model {
protected:
    static inline std::vector<std::string> fillable; // Список полей для заполнения
    static inline std::vector<std::string> fields; // Список всех полей

    std::map<std::string, std::string> attributes;


public:
    virtual ~Model() = default;

    static ModelQueryBuilder<Derived> query() {
        return ModelQueryBuilder<Derived>(Derived::table_name);
    }
  
    void setAttribute(const std::string& key, const std::string& value) {
        if (isField(key) && isFillable(key)) {
            attributes[key] = value;
        }
        else {
            std::cerr << "Field '" << key << "' is not fillable or not a valid field." << std::endl;
        }
    }

    void debugPrintAttributes() const {
        std::cout << "=== MODEL ATTRIBUTES ===\n";
        for (const auto& [key, val] : attributes) {
            std::cout << key << " = '" << val << "'\n";
        }
    }

    std::string getAttribute(const std::string& key) const {
        auto it = attributes.find(key);
        if (it != attributes.end()) {
            return it->second;
        }
        throw std::invalid_argument("Attribute '" + key + "' not found.");
    }

    void save() {
        auto database = std::make_shared<Database>();

        if (attributes.empty()) {
            std::cerr << "Attributes are empty" << std::endl;
            return;
        }

        // Формируем значения для Insert
        std::map<std::string, std::string> insertValues;
        PGconn* conn = database->getConnection();
        for (const auto& [key, value] : attributes) {
            insertValues[key] = SQLString::EscapeString(conn, value);
        }

        if (insertValues.empty()) {
            std::cerr << "Insert values are empty. Aborting save operation." << std::endl;
            return;
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

    static bool isFillable(const std::string& field) {
        return std::find(fillable.begin(), fillable.end(), field) != fillable.end();
    }

    static bool isField(const std::string& field) {
        return std::find(Derived::fields.begin(), Derived::fields.end(), field) != Derived::fields.end();
    }

    static std::shared_ptr<Derived> create(const std::map<std::string, std::string>& data) {
        auto model = std::make_shared<Derived>();
        for (const auto& [key, value] : data) {
            if (isField(key) && isFillable(key)) {
                model->setAttribute(key, value);
            } else {
                std::cerr << "Field '" << key << "' is not fillable or not a valid field (create)." << std::endl;
            }
        }
        return model;
    }

    static std::shared_ptr<Derived> find(int id) {
        try {
            auto results = query().Where("id = " + std::to_string(id)).Limit(1).get();
            if (results.empty()) {
                Logger::log("No data found by id", "WARNING");
                throw std::runtime_error("No data found for id: " + std::to_string(id));
            }
            if (results.size() > 1) {
                Logger::log("Found multiple records for single id", "ERROR");
                throw std::runtime_error("Multiple records found for id: " + std::to_string(id));
            }
            return results.front();
        }
        catch (const std::exception& e) {
            Logger::log("Finding error: " + std::string(e.what()), "ERROR");
            throw; // Пробрасываем ошибку выше
        }
    }

    static void update(int id, const std::map<std::string, std::string>& data) {
        try {
            auto db = std::make_shared<Database>();
            PGconn* conn = db->getConnection();
            SQLQueryBuilder builder(Derived::table_name);
            std::map<std::string, std::string> updateValues;
            for (const auto& [key, value] : data) {
                if (isField(key)) {
                    updateValues[key] = SQLString::EscapeString(conn, value);
                }
            }
            builder.Update(updateValues).Where("id = " + SQLString::EscapeString(conn, std::to_string(id)));
            db->execute(builder.get());
        }
        catch (const std::exception& e) {
            std::cerr << "Update failed: " << e.what() << std::endl;
        }
    }


    void delete_() {
        try {
            auto database = std::make_shared<Database>();
            PGconn* conn = database->getConnection();
            SQLQueryBuilder builder(Derived::table_name);
            builder.Delete().Where("id = " + SQLString::EscapeString(conn, this->getAttribute("id")));
            std::string query = builder.get();
            database->execute(query);
        }
        catch (const std::exception& e) {
            std::cerr << "Delete failed: " << e.what() << std::endl;
        }
    }

    static std::vector<std::shared_ptr<Derived>> where(const std::string& condition) {
        return query().Where(condition).get();
    }
};