#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>
#include <../include/libpq-fe.h>
#include <iostream>
#include <algorithm>
#include "../Database.hpp"
#include "../SQLBuilder.hpp"
#include "ModelQueryBuilder.hpp"

//   Model � �������������� CRTP
template <typename Derived>
class Model {
protected:
    static inline std::vector<std::string> fillable; // ������ ����� ��� ����������
    static inline std::vector<std::string> fields; // ������ ���� �����

public:
    std::map<std::string, std::string> attributes;

    virtual ~Model() = default;

    // ����� save
    void save() {
        // ������ ����������� � ���� ������
        auto database = std::make_shared<Database>();

        // ���������, ���� �� �������� ��� ����������
        if (attributes.empty()) {
            std::cerr << "Attributes are empty" << std::endl;
            return;
        }

        // ��������� ������ ����� � ��������
        std::string fields_;
        std::string values_;
        std::string updateClause;

        for (const auto& [key, value] : attributes) {
            fields_ += key + ", ";
            values_ += "'" + value + "', ";

            // ��������� ����� ������� ��� ���������� (����� id)
            if (key != "id") {
                updateClause += key + " = EXCLUDED." + key + ", ";
            }
        }

        // ������� ��������� ������� � ������
        if (!fields_.empty()) {
            fields_.erase(fields_.size() - 2); // ������� ", "
            values_.erase(values_.size() - 2); // ������� ", "
        }
        if (!updateClause.empty()) {
            updateClause.erase(updateClause.size() - 2); // ������� ", "
        }

        // ��������� SQL-������
        std::string query = "INSERT INTO " + Derived::table_name + " (" + fields_ + ") VALUES (" + values_ + ") "
            "ON CONFLICT (id) DO UPDATE SET " + updateClause + ";";

        // ��������� ������
        try {
            database->execute(query);
        }
        catch (const std::exception& e) {
            std::cerr << "Bad request" << e.what() << std::endl;
        }
    }

    // ������������� �������� ��������
    void setAttribute(const std::string& key, const std::string& value) {
        if (isField(key)) {
            this->attributes[key] = value; // ���������� this->attributes
        }
        else {
            std::cerr << "Field '" << key << "' is not fillable." << std::endl;
        }
    }

    // ������� ��� ��������
    void printAttributes() const {
        for (const auto& [key, value] : attributes) {
            std::cout << key << ": " << value << std::endl;
        }
    }

    // ��������� ������ ������� �� ������ ���� ������
    static std::shared_ptr<Model<Derived>> create(const std::map<std::string, std::string>& data) {
        auto model = std::make_shared<Derived>();
        for (const auto& [key, value] : data) {
            if (Derived::isField(key)) {
                model->setAttribute(key, value);
                std::cout << key << " = " << value << std::endl;
            }
            else {
                std::cerr << "Field '" << key << "' is not fillable." << std::endl;
                return nullptr;
            }
        }
        return model;
    }

    static std::shared_ptr<Derived> create(std::string data) {
        auto model = std::make_shared<Derived>();

        // ��������� ������ ������ �� ��������� ��������
        std::istringstream iss(data);
        std::string value;
        for (size_t i = 0; i < Derived::fields.size(); ++i) {
            if (std::getline(iss, value, ' ')) {
                model->setAttribute(Derived::fields[i], value);
            }
            else {
                std::cerr << "Not enough data for field: " << Derived::fields[i] << std::endl;
                return nullptr;
            }
        }

        return model;
    }

    // ���������, �������� �� ���� fillable
    static bool isFillable(const std::string& field) {
        return std::find(Derived::fillable.begin(), Derived::fillable.end(), field) != Derived::fillable.end();
    }

    static bool isField(const std::string& field) {
        return std::find(Derived::fields.begin(), Derived::fields.end(), field) != Derived::fields.end();
    }


    // ���������� �������� ��������
    std::string getAttribute(const std::string& key) const {
        if (attributes.find(key) != attributes.end()) {
            return attributes.at(key);
        }
        throw std::invalid_argument("Attribute '" + key + "' not found.");
    }

    // ����������� ���������� ��� �������� ����� �������
    static inline std::string table_name = "default_table";

    // ������ � �������
    static std::shared_ptr<Derived> find(int id) {
        try {
            // ����������� � ���� ������
            auto database = std::make_shared<Database>();

            // ��������� ������ �����
            std::string fields_;
            for (size_t i = 0; i < Derived::fields.size(); ++i) {
                fields_ += Derived::fields[i];
                if (i < Derived::fields.size() - 1) {
                    fields_ += ", "; // ��������� ������� ������ ����� ������
                }
            }

            SQLQueryBuilder sqlb(Derived::table_name);
            std::string data = sqlb.Select(Derived::fields).Where("id = " + std::to_string(id)).get();

            //std::string sql = "SELECT " + fields_ + " FROM " + Derived::table_name + " WHERE id = " + std::to_string(id);
            //std::string data = database->query(sql);

            // ���������, ��� ������ �� ������
            if (data.empty()) {
                std::cerr << "No data found for id: " << id << std::endl;
                return nullptr;
            }

            //std::cout << "Raw data: " << data << std::endl;
            //std::cout << "End of data for id: " << id << std::endl;
            //std::cout << "Executed SQL: " << sql << std::endl;


            return Derived::create(data);
        }
        catch (const std::exception& e) {
            std::cerr << "Error in read: " << e.what() << std::endl;
            return nullptr;
        }
    }

    static void update(int id, const std::map<std::string, std::string>& data) {
        try{
            auto model = Derived::find(id);
            if (model) {
                for (const auto& [key, value] : data) {
                    model->setAttribute(key, value);
                }

                model->save();
            }
        }
        catch(const std::exception& e){
            std::cerr << "Error in read: " << e.what() << std::endl;
        }
    }

    static void delete_(int id) {
        try {
            auto database = std::make_shared<Database>();

            SQLQueryBuilder sqlb(Derived::table_name);
            sqlb.Delete().Where("id = " + std::to_string(id)).get();

            //std::string query = "DELETE FROM " + Derived::table_name + " WHERE id = " + std::to_string(id) + ";";
            //database->execute(query);

            std::cout << "Record with id " << id << " deleted successfully from table: " << Derived::table_name << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error deleting record with id " << id << ": " << e.what() << std::endl;
        }
    }

    static std::vector<std::shared_ptr<Derived>> where(const std::string& condition) {
        std::vector<std::shared_ptr<Derived>> results;

        try {
            // ����������� � ���� ������
            auto database = std::make_shared<Database>();

            // ��������� ������ �����
            std::string fields_;
            for (size_t i = 0; i < Derived::fields.size(); ++i) {
                fields_ += Derived::fields[i];
                if (i < Derived::fields.size() - 1) {
                    fields_ += ", "; // ��������� ������� ������ ����� ������
                }
            }

            // ��������� SQL-������
            SQLQueryBuilder sqlb(Derived::table_name);
            std::string data = sqlb.Select(Derived::fields)
                                   .Where(condition)
                                   .get();

            //std::string sql = "SELECT " + fields_ + " FROM " + Derived::table_name + " WHERE " + condition;
            //std::string data = database->query(sql);

            // ���������, ��� ������ �� ������
            if (data.empty()) {
                std::cerr << "No data found for condition: " << condition << std::endl;
                return results;
            }

            // ��������� ������ �� ������ (������������, ��� ������ ������ � ��� ��������� ������)
            std::istringstream iss(data);
            std::string line;
            while (std::getline(iss, line)) {
                auto model = Derived::create(line);
                if (model) {
                    results.push_back(model);
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error in where: " << e.what() << std::endl;
        }

        return results;
    }

    static SQLQueryBuilder query() {
        return SQLQueryBuilder(Derived::table_name);
    }

    static std::vector<std::shared_ptr<Derived>> get(SQLQueryBuilder& builder) {
        auto database = std::make_shared<Database>();
        auto rows = database->queryToVector(builder.get());
        std::vector<std::shared_ptr<Derived>> result;
        for (const auto& row : rows) {
            auto model = Derived::create(row);
            if (model) result.push_back(model);
        }
        return result;
    }

    static ModelQueryBuilder<Derived> Select(const std::vector<std::string>& columns) {
        ModelQueryBuilder<Derived> builder(Derived::table_name);
        return builder.Select(columns);
    }
    static ModelQueryBuilder<Derived> Where(const std::string& condition) {
        ModelQueryBuilder<Derived> builder(Derived::table_name);
        return builder.Where(condition);
    }
    static ModelQueryBuilder<Derived> OrderBy(const std::vector<std::string>& columns) {
        ModelQueryBuilder<Derived> builder(Derived::table_name);
        return builder.OrderBy(columns);
    }
    static ModelQueryBuilder<Derived> Limit(int value) {
        ModelQueryBuilder<Derived> builder(Derived::table_name);
        return builder.Limit(value);
    }

};