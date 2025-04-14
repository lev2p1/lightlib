#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <libpq-fe.h>
#include <map>
#include "SQLString.hpp"
#include "../vendor/Handlers/ENV.hpp"
#include "../vendor/Debug/Logger.hpp"

class Database {
public:
    // Конструктор (подключается к базе данных)
    Database();

    // Деструктор (закрывает соединение)
    ~Database();

    // Выполнение SQL-запроса (без возврата результата)
    void execute(const std::string& sql);

    // Выполнение SQL-запроса с возвратом результата
    std::string query(const std::string& sql);

    std::map<std::string, std::string> queryMap(const std::string& sql_template, const std::vector<std::string>& params);
    std::vector<std::map<std::string, std::string>> queryToVector(const std::string& sql_template, const std::vector<std::string>& params);

    PGconn* getConnection() const { return conn_; }

private:
    PGconn* conn_; // Соединение с базой данных
};

//5433 | 5432

// Конструктор (подключается к базе данных)
inline Database::Database() {
    std::string connection_string = "host=" + ENV::env_variables["DB_HOST"] + " user=" + ENV::env_variables["DB_USERNAME"] + " password=" + ENV::env_variables["DB_PASSWORD"] + " dbname=" + ENV::env_variables["DB_DATABASE"] + " client_encoding=UTF8";
    conn_ = PQconnectdb(connection_string.c_str());
    Logger::log("Successfuly connected to database " + ENV::env_variables["DB_DATABASE"], "INFO");
    if (PQstatus(conn_) != CONNECTION_OK) {
        Logger::log("Connection failed: " + std::string(PQerrorMessage(conn_)), "ERROR");
        throw std::runtime_error("Connection failed: " + std::string(PQerrorMessage(conn_)));
    }
}

inline Database::~Database() {
    if (conn_) {
        PQfinish(conn_);
    }
}

inline void Database::execute(const std::string& sql) {
    PGresult* res = PQexec(conn_, sql.c_str());
    Logger::log("SQL Query: " + sql, "INFO");
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        Logger::log(std::string(PQerrorMessage(conn_)), "ERROR");
        PQclear(res);
        throw std::runtime_error(std::string(PQerrorMessage(conn_)));
    }

    PQclear(res);
}
// Выполнение SQL-запроса с возвратом результата
inline std::string Database::query(const std::string& sql) {
    PGresult* res = PQexec(conn_, sql.c_str());

    // Проверяем результат выполнения запроса
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        Logger::log(std::string(PQerrorMessage(conn_)), "ERROR");
        PQclear(res);
        throw std::runtime_error(std::string(PQerrorMessage(conn_)));
    }

    // Обрабатываем результат
    std::string result;
    int rows = PQntuples(res);
    int cols = PQnfields(res);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result += PQgetvalue(res, i, j);
            result += " ";
        }
        result += "\n";
    }

    PQclear(res);
    Logger::log("Successfull query execution", "INFO");
    return result;
}

inline std::vector<std::map<std::string, std::string>> Database::queryToVector(
    const std::string& sql_template,
    const std::vector<std::string>& params = {})  // Необязательный параметр
{
    // Проверка соответствия количества параметров
    size_t param_count = std::count(sql_template.begin(), sql_template.end(), '?');
    if (param_count != params.size()) {
        throw std::runtime_error("Parameter count mismatch. Expected " +
            std::to_string(param_count) +
            ", got " + std::to_string(params.size()));
    }

    // Формируем итоговый запрос
    std::string sql = sql_template;
    for (const auto& param : params) {
        size_t pos = sql.find("?");
        if (pos != std::string::npos) {
            std::string escaped_param = SQLString::EscapeString(conn_, param);
            sql.replace(pos, 1, escaped_param);
        }
    }

    PGresult* res = PQexec(conn_, sql.c_str());
    Logger::log("SQL Query: " + sql, "INFO");

    // Проверяем результат выполнения
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        Logger::log(std::string(PQerrorMessage(conn_)), "ERROR");
        PQclear(res);
        throw std::runtime_error(std::string(PQerrorMessage(conn_)));
    }

    // Обработка результатов (без изменений)
    std::vector<std::map<std::string, std::string>> result;
    int rows = PQntuples(res);
    int cols = PQnfields(res);

    std::vector<std::string> columnNames;
    for (int j = 0; j < cols; ++j) {
        columnNames.push_back(PQfname(res, j));
    }

    for (int i = 0; i < rows; ++i) {
        std::map<std::string, std::string> row;
        for (int j = 0; j < cols; ++j) {
            row[columnNames[j]] = PQgetvalue(res, i, j) ? PQgetvalue(res, i, j) : "";
        }
        result.push_back(row);
    }

    PQclear(res);
    return result;
}

inline std::map<std::string, std::string> Database::queryMap(
    const std::string& sql_template,
    const std::vector<std::string>& params = {})  // Необязательный параметр
{
    // Проверка соответствия количества параметров
    size_t param_count = std::count(sql_template.begin(), sql_template.end(), '?');
    if (param_count != params.size()) {
        throw std::runtime_error("Parameter count mismatch. Expected " +
            std::to_string(param_count) +
            ", got " + std::to_string(params.size()));
    }

    // Формируем итоговый запрос
    std::string sql = sql_template;
    for (const auto& param : params) {
        size_t pos = sql.find("?");
        if (pos != std::string::npos) {
            std::string escaped_param = SQLString::EscapeString(conn_, param);
            sql.replace(pos, 1, escaped_param);
        }
    }

    PGresult* res = PQexec(conn_, sql.c_str());
    Logger::log("SQL Query: " + sql, "INFO");
    std::map<std::string, std::string> row;

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        Logger::log(std::string(PQerrorMessage(conn_)), "ERROR");
        PQclear(res);
        throw std::runtime_error(std::string(PQerrorMessage(conn_)));
    }

    if (PQntuples(res) > 0) {
        for (int i = 0; i < PQnfields(res); i++) {
            row[PQfname(res, i)] = PQgetvalue(res, 0, i) ? PQgetvalue(res, 0, i) : "";
        }
    }

    PQclear(res);
    return row;
}