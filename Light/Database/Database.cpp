#include "Database.hpp"
#include <iostream>


// Конструктор (подключается к базе данных)
Database::Database() {
    std::string connection_string = "host=" + ENV::env_variables["DB_HOST"] + " user=" + ENV::env_variables["DB_USERNAME"] + " password=" + ENV::env_variables["DB_PASSWORD"] + " dbname=" + ENV::env_variables["DB_DATABASE"] + " client_encoding=UTF8";
    conn_ = PQconnectdb(connection_string.c_str());
    if (PQstatus(conn_) != CONNECTION_OK) {
        throw std::runtime_error("Connection failed: " + std::string(PQerrorMessage(conn_)));
    }
}

Database::~Database() {
    if (conn_) {
        PQfinish(conn_);
    }
}

void Database::execute(const std::string& sql) {
    setlocale(LC_ALL, "ru");
    PGresult* res = PQexec(conn_, sql.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        throw std::runtime_error("Execution failed: " + std::string(PQerrorMessage(conn_)));
    }

    PQclear(res);
}

// Выполнение SQL-запроса с возвратом результата
std::string Database::query(const std::string& sql) {
    PGresult* res = PQexec(conn_, sql.c_str());

    // Проверяем результат выполнения запроса
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error_message = "Query failed: " + std::string(PQerrorMessage(conn_));
        PQclear(res);
        throw std::runtime_error(error_message);
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
    return result;
}