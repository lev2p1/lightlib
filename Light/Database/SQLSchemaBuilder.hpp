#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <memory>
#include "../Database/Database.hpp"

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
    // Конструктор
    explicit SQLSchemaBuilder(const std::string& table) : table(table) {}

    // Добавление столбца
    SQLSchemaBuilder& AddColumn(const std::string& columnDefinition) {
        columns.push_back(columnDefinition);
        return *this;
    }

    // Добавление первичного ключа
    SQLSchemaBuilder& AddPrimaryKey(const std::string& column) {
        primaryKeys.push_back(column);
        return *this;
    }

    // Добавление внешнего ключа
    SQLSchemaBuilder& AddForeignKey(const std::string& column, const std::string& referenceTable, const std::string& referenceColumn) {
        foreignKeys.push_back(column + " REFERENCES " + referenceTable + "(" + referenceColumn + ")");
        return *this;
    }

    // Добавление индекса
    SQLSchemaBuilder& AddIndex(const std::string& indexName, const std::vector<std::string>& columns) {
        std::ostringstream indexColumns;
        for (size_t i = 0; i < columns.size(); ++i) {
            indexColumns << columns[i];
            if (i < columns.size() - 1) {
                indexColumns << ", ";
            }
        }
        indexes.push_back("CREATE INDEX " + indexName + " ON " + table + " (" + indexColumns.str() + ")");
        return *this;
    }

    // Добавление уникального ограничения
    SQLSchemaBuilder& AddUniqueConstraint(const std::string& constraintName, const std::vector<std::string>& columns) {
        std::ostringstream uniqueColumns;
        for (size_t i = 0; i < columns.size(); ++i) {
            uniqueColumns << columns[i];
            if (i < columns.size() - 1) {
                uniqueColumns << ", ";
            }
        }
        uniqueConstraints.push_back("ALTER TABLE " + table + " ADD CONSTRAINT " + constraintName + " UNIQUE (" + uniqueColumns.str() + ")");
        return *this;
    }

    // Удаление столбца
    SQLSchemaBuilder& DropColumn(const std::string& column) {
        dropColumns.push_back(column);
        return *this;
    }

    // Изменение столбца
    SQLSchemaBuilder& AlterColumn(const std::string& columnDefinition) {
        alterColumns.push_back(columnDefinition);
        return *this;
    }

    // Построение SQL-запроса для создания таблицы
    std::string CreateTable() {

        std::ostringstream query;
        Database db;

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

        query << ")";

        return db.query( query.str());
    }

    // Построение SQL-запроса для изменения таблицы
    std::string AlterTable() {
        std::ostringstream query;

        query << "ALTER TABLE " << table << " ";

        // Удаление столбцов
        for (size_t i = 0; i < dropColumns.size(); ++i) {
            query << "DROP COLUMN " << dropColumns[i];
            if (i < dropColumns.size() - 1 || !alterColumns.empty()) {
                query << ", ";
            }
        }

        // Изменение столбцов
        for (size_t i = 0; i < alterColumns.size(); ++i) {
            query << "MODIFY COLUMN " << alterColumns[i];
            if (i < alterColumns.size() - 1) {
                query << ", ";
            }
        }

        return query.str();
    }

    // Построение SQL-запроса для удаления таблицы
    std::string DropTable() {
        return "DROP TABLE " + table;
    }

    // Построение SQL-запросов для создания индексов
    std::vector<std::string> CreateIndexes() {
        return indexes;
    }

    // Построение SQL-запросов для добавления уникальных ограничений
    std::vector<std::string> AddUniqueConstraints() {
        return uniqueConstraints;
    }
};