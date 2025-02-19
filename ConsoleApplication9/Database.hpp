#pragma once

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <string>
#include <stdexcept>
#include <memory>

class Database {
public:
    // Конструктор (подключается к базе данных)
    Database(const std::string& host, const std::string& user, const std::string& password, const std::string& database);

    // Деструктор (закрывает соединение)
    ~Database();

    // Выполнение SQL-запроса (без возврата результата)
    void execute(const std::string& sql);

    // Выполнение SQL-запроса с возвратом результата
    std::string query(const std::string& sql);

private:
    sql::mysql::MySQL_Driver* driver_; // Драйвер MySQL
    std::unique_ptr<sql::Connection> conn_; // Соединение с базой данных
};
