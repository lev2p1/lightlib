#pragma once

#include "BaseMigration.hpp"
#include "Migrations.hpp"

class MigrationPostsCreate : public BaseMigration<MigrationPostsCreate>{

    static inline std::string table = "posts";
    
public:

    static std::string up() {
        SQLSchemaBuilder schemaBuilder(table);
        return schemaBuilder
            .AddColumn("id SERIAL PRIMARY KEY") // Используем SERIAL и PRIMARY KEY
            .AddColumn("name VARCHAR(255) NOT NULL")
            .AddColumn("email VARCHAR(255) NOT NULL")
            .AddUniqueConstraint("unique_email", { "email" })
            .AddIndex("idx_name", { "name" })
            .CreateTable();
    }

};