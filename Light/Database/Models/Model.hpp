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

template <typename Derived>
class Model {
protected:
    static inline std::vector<std::string> fillable;
    static inline std::vector<std::string> fields;
    static inline std::string table_name = "default_table";

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

    // ������������� �������� ��������
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

        std::string fields_str;
        std::string values_str;
        std::string update_clause;

        for (const auto& [key, value] : attributes) {
            fields_str += key + ", ";
            values_str += "'" + escape(value) + "', ";

            if (key != "id") {
                update_clause += key + " = EXCLUDED." + key + ", ";
            }
        }

        // ������� ��������� ", "
        if (!fields_str.empty()) {
            fields_str.erase(fields_str.size() - 2);
            values_str.erase(values_str.size() - 2);
        }
        if (!update_clause.empty()) {
            update_clause.erase(update_clause.size() - 2);
        }

        std::string query = "INSERT INTO " + Derived::table_name +
            " (" + fields_str + ") VALUES (" + values_str + ") " +
            "ON CONFLICT (id) DO UPDATE SET " + update_clause + ";";

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

    // ����� �� ID
    static std::shared_ptr<Derived> find(int id) {
        try {

            auto db = std::make_shared<Database>();
            std::string sql = "SELECT * FROM " + Derived::table_name + " WHERE id = " + std::to_string(id);

            auto db_data = db->queryMap(sql);

            if (db_data.empty()) {
                Logger::log("No data found by id", "WARNING");
                return nullptr;
            }

            auto model = std::make_shared<Derived>();

            for (const auto& [field, value] : db_data) {
                model->setAttribute(field, value);
            }
            return model;

        }
        catch (const std::exception& e) {
            Logger::log("Finding error", "ERROR");
            return nullptr;
        }
    }

    // ���������� ������
    static void update(int id, const std::map<std::string, std::string>& data) {
        try {
            auto model = Derived::find(id);
            if (model) {
                for (const auto& [key, value] : data) {
                    if (isField(key)) {
                        model->setAttribute(key, value);
                    }
                }
                model->save();
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Update failed: " << e.what() << std::endl;
        }
    }


    void delete_() {
        try {
            auto database = std::make_shared<Database>();
            std::string query = "DELETE FROM " + Derived::table_name +
                " WHERE id = " + this->getAttribute("id");
            database->execute(query);
        }
        catch (const std::exception& e) {
            std::cerr << "Delete failed: " << e.what() << std::endl;
        }
    }

    // ����� �� �������
    static std::vector<std::shared_ptr<Derived>> where(const std::string& condition) {
        std::vector<std::shared_ptr<Derived>> results;

        try {
            auto database = std::make_shared<Database>();
            std::string query = "SELECT * FROM " + Derived::table_name +
                " WHERE " + condition;

            // ������������, ��� Database::query ���������� ������ map'��
            auto data_list = database->queryToVector(query);

            for (const auto& data : data_list) {
                auto model = Derived::create(data);
                if (model) {
                    results.push_back(model);
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Where failed: " << e.what() << std::endl;
        }

        return results;
    }
};