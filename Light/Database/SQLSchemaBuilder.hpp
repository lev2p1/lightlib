#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <memory>
#include "Database.hpp"  // Подключите ваш заголовочный файл Database


class SQLSchemaBuilder {
private:
    std::string table;
    std::vector<std::string> columns;
    std::vector<std::string> primaryKeys;
    std::vector<std::string> foreignKeys;
    std::vector<std::string> indexes;
    std::vector<std::string> uniqueConstraints;
    std::vector<std::string> dropColumns;
    std::vector<std::string> alterColumns;

public:
    explicit SQLSchemaBuilder(const std::string& table) : table(table) {}

    SQLSchemaBuilder& AddColumn(const std::string& columnDefinition) {
        columns.push_back(columnDefinition);
        return *this;
    }

    SQLSchemaBuilder& AddPrimaryKey(const std::string& column) {
        primaryKeys.push_back(column);
        return *this;
    }

    SQLSchemaBuilder& AddForeignKey(const std::string& column, const std::string& referenceTable, const std::string& referenceColumn) {
        foreignKeys.push_back(column + " REFERENCES " + referenceTable + "(" + referenceColumn + ")");
        return *this;
    }

    std::string AddIndex(const std::string& indexName, const std::vector<std::string>& columns) {
        std::ostringstream indexColumns;
        for (size_t i = 0; i < columns.size(); ++i) {
            indexColumns << columns[i];
            if (i < columns.size() - 1) {
                indexColumns << ", ";
            }
        }
        return "CREATE INDEX " + indexName + " ON " + table + " (" + indexColumns.str() + ");";
    }

    std::string AddUniqueConstraint(const std::string& constraintName, const std::vector<std::string>& columns) {
        std::ostringstream uniqueColumns;
        for (size_t i = 0; i < columns.size(); ++i) {
            uniqueColumns << columns[i];
            if (i < columns.size() - 1) {
                uniqueColumns << ", ";
            }
        }
        return "ALTER TABLE " + table + " ADD CONSTRAINT " + constraintName + " UNIQUE (" + uniqueColumns.str() + ");";
    }

    SQLSchemaBuilder& DropColumn(const std::string& column) {
        dropColumns.push_back(column);
        return *this;
    }

    SQLSchemaBuilder& AlterColumn(const std::string& columnDefinition) {
        alterColumns.push_back(columnDefinition);
        return *this;
    }

    std::string CreateTable() {
        std::ostringstream query;

        // Создаем таблицу
        query << "CREATE TABLE " << table << " (";

        // Добавляем столбцы
        for (size_t i = 0; i < columns.size(); ++i) {
            query << columns[i];
            if (i < columns.size() - 1 || !primaryKeys.empty() || !foreignKeys.empty()) {
                query << ", ";
            }
        }

        // Добавляем первичные ключи
        if (!primaryKeys.empty()) {
            query << "PRIMARY KEY (";
            for (size_t i = 0; i < primaryKeys.size(); ++i) {
                query << primaryKeys[i];
                if (i < primaryKeys.size() - 1) {
                    query << ", ";
                }
            }
            query << ")";
            if (!foreignKeys.empty()) {
                query << ", ";
            }
        }

        // Добавляем внешние ключи
        for (size_t i = 0; i < foreignKeys.size(); ++i) {
            query << "FOREIGN KEY (" << foreignKeys[i] << ")";
            if (i < foreignKeys.size() - 1) {
                query << ", ";
            }
        }

        query << ");";

        return query.str();
    }

    std::string DropTable() {
        return "DROP TABLE " + table + " CASCADE;";
    }
};