//#pragma once
//
//#include "BaseMigration.hpp"
//#include "Migrations.hpp"
//
//class MigrationMigrationsCreate : public BaseMigration<MigrationMigrationsCreate> {
//
//    static inline std::string table = "migrations";
//
//public:
//
//    static std::string up() {
//        SQLSchemaBuilder schemaBuilder(table);
//        return schemaBuilder
//            .AddColumn("id SERIAL PRIMARY KEY") // Используем SERIAL и PRIMARY KEY
//            .AddColumn("name VARCHAR(255) NOT NULL")
//            .AddIndex("idx_name", { "name" })
//            .CreateTable();
//    }
//
//};