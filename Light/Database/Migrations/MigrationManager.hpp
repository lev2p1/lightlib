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
    Database& db;  // ��� ����� ��� ������ � ����� ������
    std::unordered_set<std::string> executedMigrations;  // ����������� ��������

    // ���������, ��������� �� ��������
    bool isMigrationExecuted(const std::string& name) {
        return executedMigrations.find(name) != executedMigrations.end();
    }

    // ��������� �������� � ������ �����������
    void markMigrationAsExecuted(const std::string& name) {
        db.execute("INSERT INTO migrations (name) VALUES ('" + name + "');");
        executedMigrations.insert(name);
    }

    // ������� �������� �� ������ �����������
    void unmarkMigrationAsExecuted(const std::string& name) {
        db.execute("DELETE FROM migrations WHERE name = '" + name + "';");
        executedMigrations.erase(name);
    }

    // ��������� SQL-�������
    void executeQueries(const std::vector<std::string>& queries) {
        for (const auto& query : queries) {
            db.execute(query);

        }
    }

public:
    MigrationManager(Database& db) : db(db) {
        try {
            auto result = db.queryToVector("SELECT name FROM migrations;");

            // ������������ ���������
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

    // ��������� ��������
    template <typename Migration>
    void migrate() {
        std::string name = typeid(Migration).name();

        if (isMigrationExecuted(name)) {
            std::cout << "Migration already executed: " << name << std::endl;
            return;
        }

        try {
            // ��������� ��������
            auto queries = Migration::up();  // �������� ��� �������

            // ��������� ��� �������
            for (const auto& query : queries) {
                db.execute(query);
            }

            markMigrationAsExecuted(name);  // �������� �������� ��� �����������
            std::cout << "Migration executed: " << name << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Migration failed: " << name << " - " << e.what() << std::endl;
            throw;
        }
    }

    // ���������� ��������
    template <typename Migration>
    void rollback() {
        std::string name = typeid(Migration).name();

        if (!isMigrationExecuted(name)) {
            std::cout << "Migration not executed: " << name << std::endl;
            return;
        }

        try {
            //db.beginTransaction();  // �������� ����������

            // ��������� ����� ��������
            std::string mainQuery = Migration::down();
            db.execute(mainQuery);

            unmarkMigrationAsExecuted(name);  // ������� �������� �� ������ �����������
            //db.commitTransaction();           // ��������� ����������
            std::cout << "Migration rolled back: " << name << std::endl;
        }
        catch (const std::exception& e) {
            //db.rollbackTransaction();  // ���������� ���������� � ������ ������
            std::cerr << "Rollback failed: " << name << " - " << e.what() << std::endl;
            throw;
        }
    }

    // ��������� ��� �������� �� �������
    template <typename... Migrations>
    void migrateAll() {
        (migrate<Migrations>(), ...);
    }

    // ���������� ��� �������� � �������� �������
    template <typename... Migrations>
    void rollbackAll() {
        (rollback<Migrations>(), ...);
    }

    void Initialize() {
        this->migrateAll<
            MigrationUsersCreate
        >();
    }
    

};