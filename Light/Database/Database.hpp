#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <libpq-fe.h>
#include <map>
#include "../vendor/Handlers/ENV.hpp"

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

    std::vector<std::map<std::string, std::string>> queryToVector(const std::string& sql);

private:
    PGconn* conn_; // Соединение с базой данных
};

//5433 | 5432