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
#include "migration_organizations_table.hpp"



class MigrationManager {
private:
    Database& db;
    std::unordered_set<std::string> executedMigrations;

    bool isMigrationExecuted(const std::string& name) {
        return executedMigrations.find(name) != executedMigrations.end();
    }

    void markMigrationAsExecuted(const std::string& name) {
        db.execute("INSERT INTO migrations (name) VALUES ('" + name + "');");
        executedMigrations.insert(name);
    }

    void unmarkMigrationAsExecuted(const std::string& name) {
        db.execute("DELETE FROM migrations WHERE name = '" + name + "';");
        executedMigrations.erase(name);
    }

    void executeQueries(const std::vector<std::string>& queries) {
        for (const auto& query : queries) {
            db.execute(query);

        }
    }

public:
    MigrationManager(Database& db) : db(db) {
        try {
            auto result = db.queryToVector("SELECT name FROM migrations;");

            for (const auto& row : result) {
                auto it = row.find("name");
                if (it != row.end()) {
                    executedMigrations.insert(it->second); 
                }
                else {
                    Logger::log("Column 'name' not found in row", "Warning");
                }
            }
        }
        catch (const std::exception& e) {
            Logger::log("Error loading migrations", "ERROR");
            throw;  
        }
    }

    template <typename Migration>
    void migrate() {
        std::string name = typeid(Migration).name();

        if (isMigrationExecuted(name)) {
            Logger::log("Migration already executed: " + name, "INFO");
            return;
        }

        try {
         
            auto queries = Migration::up();  

            for (const auto& query : queries) {
                db.execute(query);
            }

            markMigrationAsExecuted(name); 
            Logger::log("Migration executed: " + name, "SUCCESS");
        }
        catch (const std::exception& e) {
            Logger::log("Migration failed: " + name + " - " + e.what(), "ERROR");
            throw;
        }
    }

    template <typename Migration>
    void rollback() {
        std::string name = typeid(Migration).name();

        if (!isMigrationExecuted(name)) {
            Logger::log("Migration not executed: " + name, "ERROR");
            return;
        }

        try {
            std::string mainQuery = Migration::down();
            db.execute(mainQuery);

            unmarkMigrationAsExecuted(name);
            Logger::log("Migration rolled back: " + name, "INFO");
        }
        catch (const std::exception& e) {
            Logger::log("Rollback failed: " + name + " - " + e.what(), "ERROR");
            throw;
        }
    }

    template <typename... Migrations>
    void migrateAll() {
        (migrate<Migrations>(), ...);
    }

    template <typename... Migrations>
    void rollbackAll() {
        (rollback<Migrations>(), ...);
    }

    void Initialize() {
        this->migrateAll<
            MigrationUsersCreate,
            MigrationOrganizationsCreate
        >();
    }
    

};