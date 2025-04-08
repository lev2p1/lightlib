#pragma once

#include "BaseMigration.hpp"

class MigrationMigrationsCreate : public BaseMigration<MigrationMigrationsCreate> {
public:
    static std::string up() {
        SQLSchemaBuilder builder("migrations");
        std::string full_query;

        full_query += builder
            .AddColumn("id SERIAL PRIMARY KEY")
            .AddColumn("name VARCHAR(255) NOT NULL")
            .CreateTable() + "; ";

        full_query += builder.AddIndex("idx_name", { "name" }) + "; ";

        return full_query;
    }

    static std::string down() {
        SQLSchemaBuilder builder("migrations");
        return builder.DropTable();
    }
};