#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <memory>

class SQLQueryBuilder {
private:
    std::string table;
    std::vector<std::string> selectColumns;
    std::vector<std::string> whereConditions;
    std::vector<std::string> orderByColumns;
    std::vector<std::string> groupByColumns;
    std::string limit;
    std::string offset;
    std::map<std::string, std::string> joins;
    std::map<std::string, std::string> insertValues;
    std::map<std::string, std::string> updateValues;
    bool isDeleteQuery = false;
    bool isFirstConditionInGroup = true;

public:
    explicit SQLQueryBuilder(const std::string& table) : table(table) {}

    // ���������� �������� ��� SELECT
    SQLQueryBuilder& Select(const std::vector<std::string>& columns) {
        selectColumns = columns;
        return *this;
    }

    // ��������� ����� ��� DELETE �������
    SQLQueryBuilder& Delete() {
        isDeleteQuery = true;
        return *this;
    }

    // ���������� ������� WHERE
    SQLQueryBuilder& Where(const std::string& condition, const std::string& logicalOperator = "AND") {
        if (!whereConditions.empty() && !isFirstConditionInGroup) {
            whereConditions.push_back(logicalOperator + " " + condition);
        }
        else {
            whereConditions.push_back(condition);
            isFirstConditionInGroup = false;
        }
        return *this;
    }

    // ������ ������ �������
    SQLQueryBuilder& BeginGroup(const std::string& logicalOperator = "AND") {
        if (!whereConditions.empty()) {
            whereConditions.push_back(logicalOperator + " (");
        }
        else {
            whereConditions.push_back("(");
        }
        isFirstConditionInGroup = true;
        return *this;
    }

    // ����� ������ �������
    SQLQueryBuilder& EndGroup() {
        whereConditions.push_back(")");
        isFirstConditionInGroup = false;
        return *this;
    }

    // ���������� ���������� ORDER BY
    SQLQueryBuilder& OrderBy(const std::vector<std::string>& columns) {
        orderByColumns = columns;
        return *this;
    }

    // ���������� ����������� GROUP BY
    SQLQueryBuilder& GroupBy(const std::vector<std::string>& columns) {
        groupByColumns = columns;
        return *this;
    }

    // ���������� LIMIT
    SQLQueryBuilder& Limit(int value) {
        limit = " LIMIT " + std::to_string(value);
        return *this;
    }

    // ���������� OFFSET
    SQLQueryBuilder& Offset(int value) {
        offset = " OFFSET " + std::to_string(value);
        return *this;
    }

    // ���������� JOIN
    SQLQueryBuilder& Join(const std::string& table, const std::string& onCondition, const std::string& joinType = "INNER") {
        joins[table] = joinType + " JOIN " + table + " ON " + onCondition;
        return *this;
    }

    // ���������� ������ ��� INSERT
    SQLQueryBuilder& Insert(const std::map<std::string, std::string>& values) {
        insertValues = values;
        return *this;
    }

    SQLQueryBuilder& Update(const std::map<std::string, std::string>& values) {
        updateValues = values;
        return *this;
    }

    //  SQL-
    std::string get() {
        std::ostringstream query;

        if (!insertValues.empty()) {
            // INSERT ������
            query << "INSERT INTO " << table << " (";
            for (auto it = insertValues.begin(); it != insertValues.end(); ++it) {
                query << it->first;
                if (std::next(it) != insertValues.end()) {
                    query << ", ";
                }
            }
            query << ") VALUES (";
            for (auto it = insertValues.begin(); it != insertValues.end(); ++it) {
                query << it->second;
                if (std::next(it) != insertValues.end()) {
                    query << ", ";
                }
            }
            query << ")";
        }
        else if (!updateValues.empty()) {
            // UPDATE query
            query << "UPDATE " << table << " SET ";
            for (auto it = updateValues.begin(); it != updateValues.end(); ++it) {
                query << it->first << " = " << it->second;
                if (std::next(it) != updateValues.end()) {
                    query << ", ";
                }
            }

            // WHERE conditions
            if (!whereConditions.empty()) {
                query << " WHERE ";
                for (size_t i = 0; i < whereConditions.size(); ++i) {
                    query << whereConditions[i];
                    if (i < whereConditions.size() - 1) {
                        query << " ";
                    }
                }
            }
        }
        else if (isDeleteQuery) {
            // DELETE ������
            query << "DELETE FROM " << table;

            // WHERE �������
            if (!whereConditions.empty()) {
                query << " WHERE ";
                for (size_t i = 0; i < whereConditions.size(); ++i) {
                    query << whereConditions[i];
                    if (i < whereConditions.size() - 1) {
                        query << " ";
                    }
                }
            }

            // ORDER BY (��� ��������� ���� �������������� � DELETE)
            if (!orderByColumns.empty()) {
                query << " ORDER BY ";
                for (size_t i = 0; i < orderByColumns.size(); ++i) {
                    query << orderByColumns[i];
                    if (i < orderByColumns.size() - 1) {
                        query << ", ";
                    }
                }
            }

            // LIMIT (��� ��������� ���� �������������� � DELETE)
            if (!limit.empty()) {
                query << limit;
            }
        }
        else {
            // SELECT ������
            query << "SELECT ";
            if (selectColumns.empty()) {
                query << "*";
            }
            else {
                for (size_t i = 0; i < selectColumns.size(); ++i) {
                    query << selectColumns[i];
                    if (i < selectColumns.size() - 1) {
                        query << ", ";
                    }
                }
            }
            query << " FROM " << table;

            // JOIN
            for (const auto& [table, joinClause] : joins) {
                query << " " << joinClause;
            }

            // WHERE
            if (!whereConditions.empty()) {
                query << " WHERE ";
                for (size_t i = 0; i < whereConditions.size(); ++i) {
                    query << whereConditions[i];
                    if (i < whereConditions.size() - 1) {
                        query << " ";
                    }
                }
            }

            // GROUP BY
            if (!groupByColumns.empty()) {
                query << " GROUP BY ";
                for (size_t i = 0; i < groupByColumns.size(); ++i) {
                    query << groupByColumns[i];
                    if (i < groupByColumns.size() - 1) {
                        query << ", ";
                    }
                }
            }

            // ORDER BY
            if (!orderByColumns.empty()) {
                query << " ORDER BY ";
                for (size_t i = 0; i < orderByColumns.size(); ++i) {
                    query << orderByColumns[i];
                    if (i < orderByColumns.size() - 1) {
                        query << ", ";
                    }
                }
            }

            // LIMIT � OFFSET
            if (!limit.empty()) {
                query << limit;
            }
            if (!offset.empty()) {
                query << offset;
            }
        }

        return query.str();
    }
};