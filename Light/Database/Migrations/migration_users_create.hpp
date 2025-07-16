#pragma once

#include "BaseMigration.hpp"

class MigrationUsersCreate : public BaseMigration<MigrationUsersCreate> {
public:
    static std::vector<std::string> up() {
        SQLSchemaBuilder builder("users");
        std::vector<std::string> queries;

        // �������� ������ �� �������� �������
        queries.push_back(builder
            .AddColumn("id SERIAL PRIMARY KEY")
            .AddColumn("username VARCHAR(255) NOT NULL")
            .AddColumn("password VARCHAR(255) NOT NULL")
            .AddColumn("email VARCHAR(255) NOT NULL")
            .CreateTable());

        // �������������� ������� (������� � ���������� �����������)
        queries.push_back(builder.AddUniqueConstraint("uq_email", { "email" }));
        queries.push_back(builder.AddIndex("idx_username", { "username" }));

        return queries;
    }

    static std::string down() {
        SQLSchemaBuilder builder("users");
        return builder.DropTable();
    }
};