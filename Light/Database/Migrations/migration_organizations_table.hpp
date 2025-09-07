#pragma once

#include "BaseMigration.hpp"

class MigrationOrganizationsCreate : public BaseMigration<MigrationOrganizationsCreate> {
public:
    static std::vector<std::string> up() {
        SQLSchemaBuilder builder("organizations");
        std::vector<std::string> queries;

        queries.push_back(builder
            .AddColumn("id SERIAL PRIMARY KEY")
            .AddColumn("id_user INT NULL")
            .AddColumn("name VARCHAR(255) NOT NULL")
            .AddColumn("image VARCHAR(255) NULL")
            .CreateTable());

        return queries;
    }

    static std::string down() {
        SQLSchemaBuilder builder("organizations");
        return builder.DropTable();
    }
};
