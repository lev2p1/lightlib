#pragma once

#include "BaseMigration.hpp"

class MigrationUsersCreate : public BaseMigration<MigrationUsersCreate> {
public:
    static std::vector<std::string> up() {
        SQLSchemaBuilder builder("users");
        std::vector<std::string> queries;

        queries.push_back(builder
            .AddColumn("id SERIAL PRIMARY KEY")
            .AddColumn("username VARCHAR(255) NOT NULL")
            .AddColumn("password VARCHAR(255) NOT NULL")
            .AddColumn("salt VARCHAR(255) NOT NULL")
            .AddColumn("email VARCHAR(255) NOT NULL")
			.AddColumn("picture VARCHAR(255) NULL")
			.AddColumn("is_verified BOOLEAN DEFAULT FALSE")
			.AddColumn("verification_token VARCHAR(255) NULL")
			.AddColumn("two_fa_enabled BOOLEAN DEFAULT FALSE")
			.AddColumn("two_fa_secret VARCHAR NULL")
			.AddColumn("backup_codes TEXT NULL")
			.AddColumn("last_login TIMESTAMP NULL")
            .AddColumn("role VARCHAR(50) DEFAULT 'user'")
			.AddColumn("status VARCHAR(50) DEFAULT 'active'")
			.AddColumn("last_password_change TIMESTAMP NULL")
			.AddColumn("reset_token VARCHAR(255) NULL")
			.AddColumn("reset_token_expires_at TIMESTAMP NULL")
            .AddColumn("timezone DATE NOT NULL")
			.AddColumn("language VARCHAR(2) NOT NULL")
			.AddColumn("country VARCHAR(2) NOT NULL")
			.AddColumn("created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP")
			.AddColumn("updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP")
			.AddColumn("deleted_at TIMESTAMP NULL")
            .CreateTable());

        queries.push_back(builder.AddUniqueConstraint("uq_email", { "email" }));
        queries.push_back(builder.AddIndex("idx_username", { "username" }));

        return queries;
    }

    static std::string down() {
        SQLSchemaBuilder builder("users");
        return builder.DropTable();
    }
};