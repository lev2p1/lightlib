#pragma once

#include "BaseMigration.hpp"

class MigrationPostsCreate : public BaseMigration<MigrationPostsCreate> {
public:
    static std::vector<std::string> up() {
        SQLSchemaBuilder builder("posts");
        std::vector<std::string> queries;

        // Основной запрос на создание таблицы
        queries.push_back(builder
            .AddColumn("id SERIAL PRIMARY KEY")
            .AddColumn("text VARCHAR(255) NOT NULL")
            .AddColumn("rating VARCHAR(255) NOT NULL")
            .AddColumn("description VARCHAR(255) NOT NULL")
            .CreateTable());

        return queries;
    }

    static std::string down() {
        SQLSchemaBuilder builder("posts");
        return builder.DropTable();
    }
};