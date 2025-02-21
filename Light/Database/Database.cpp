#include "Database.hpp"
#include <iostream>
#include <windows.h>

// Конструктор (подключается к базе данных)
Database::Database(const std::string& host, const std::string& user, const std::string& password, const std::string& database) {
        std::setlocale(LC_ALL, "UTF-8");
        SetConsoleOutputCP(CP_UTF8);
        std::cout << password << "123хуй" << std::endl;
    try {
        driver_ = sql::mysql::get_mysql_driver_instance();
        std::cout << host << ' ' << user << std::endl;
        conn_.reset(driver_->connect(host, user, password));
        conn_->setSchema(database);
        std::cout << "Connected to MySQL database successfully!" << std::endl;
    }
    catch (sql::SQLException& e) {
        throw std::runtime_error("MySQL error: " + std::string(e.what()));
    }
}

// Деструктор (закрывает соединение)
Database::~Database() {
    if (conn_) {
        conn_->close();
        std::cout << "Database connection closed." << std::endl;
    }
}

// Выполнение SQL-запроса (без возврата результата)
void Database::execute(const std::string& sql) {
    try {
        std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
        stmt->execute(sql);
    }
    catch (sql::SQLException& e) {
        throw std::runtime_error("MySQL error: " + std::string(e.what()));
    }
}

// Выполнение SQL-запроса с возвратом результата
std::string Database::query(const std::string& sql) {
    try {
        std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(sql));

        std::string result;
        while (res->next()) {
            int columns = res->getMetaData()->getColumnCount();
            for (int i = 1; i <= columns; ++i) {
                result += res->getString(i) + " ";
            }
            result += "\n";
        }

        return result;
    }
    catch (sql::SQLException& e) {
        throw std::runtime_error("MySQL error: " + std::string(e.what()));
    }
}