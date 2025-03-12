#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <unordered_set>
#include <stdexcept>
#include "../Database.hpp"
#include "../SQLSchemaBuilder.hpp"
#include "migration_users_create.hpp"
#include "migration_migrations_table.hpp"



class MigrationManager {
private:
    Database& db;  // Ваш класс для работы с базой данных
    std::unordered_set<std::string> executedMigrations;  // Выполненные миграции

    // Проверяет, выполнена ли миграция
    bool isMigrationExecuted(const std::string& name) {
        return executedMigrations.find(name) != executedMigrations.end();
    }

    // Добавляет миграцию в список выполненных
    void markMigrationAsExecuted(const std::string& name) {
        db.execute("INSERT INTO migrations (name) VALUES ('" + name + "');");
        executedMigrations.insert(name);
    }

    // Удаляет миграцию из списка выполненных
    void unmarkMigrationAsExecuted(const std::string& name) {
        db.execute("DELETE FROM migrations WHERE name = '" + name + "';");
        executedMigrations.erase(name);
    }

    // Выполняет SQL-запросы
    void executeQueries(const std::vector<std::string>& queries) {
        for (const auto& query : queries) {
            db.execute(query);
        }
    }

public:
    MigrationManager(Database& db) : db(db) {
        try {
            auto result = db.queryToVector("SELECT name FROM migrations;");

            // Обрабатываем результат
            for (const auto& row : result) {
                auto it = row.find("name");
                if (it != row.end()) {
                    executedMigrations.insert(it->second); 
                }
                else {
                    std::cerr << "Warning: Column 'name' not found in row" << std::endl;
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading migrations: " << e.what() << std::endl;
            throw;  
        }
    }

    // Выполняет миграцию
    template <typename Migration>
    void migrate() {
        std::string name = typeid(Migration).name();

        if (isMigrationExecuted(name)) {
            std::cout << "Migration already executed: " << name << std::endl;
            return;
        }

        try {
            // Выполняем миграцию
            auto queries = Migration::up();  // Получаем все запросы

            // Выполняем все запросы
            for (const auto& query : queries) {
                db.execute(query);
            }

            markMigrationAsExecuted(name);  // Отмечаем миграцию как выполненную
            std::cout << "Migration executed: " << name << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Migration failed: " << name << " - " << e.what() << std::endl;
            throw;
        }
    }

    // Откатывает миграцию
    template <typename Migration>
    void rollback() {
        std::string name = typeid(Migration).name();

        if (!isMigrationExecuted(name)) {
            std::cout << "Migration not executed: " << name << std::endl;
            return;
        }

        try {
            //db.beginTransaction();  // Начинаем транзакцию

            // Выполняем откат миграции
            std::string mainQuery = Migration::down();
            db.execute(mainQuery);

            unmarkMigrationAsExecuted(name);  // Убираем миграцию из списка выполненных
            //db.commitTransaction();           // Завершаем транзакцию
            std::cout << "Migration rolled back: " << name << std::endl;
        }
        catch (const std::exception& e) {
            //db.rollbackTransaction();  // Откатываем транзакцию в случае ошибки
            std::cerr << "Rollback failed: " << name << " - " << e.what() << std::endl;
            throw;
        }
    }

    // Выполняет все миграции по порядку
    template <typename... Migrations>
    void migrateAll() {
        (migrate<Migrations>(), ...);
    }

    // Откатывает все миграции в обратном порядке
    template <typename... Migrations>
    void rollbackAll() {
        (rollback<Migrations>(), ...);
    }

    void Initialize() {
        std::cout << "Хуйня здесь";
        this->migrateAll<
            MigrationUsersCreate
        >();
    }
    

};