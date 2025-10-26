#pragma once

#include "BaseMigration.hpp"

class MigrationBackupCodes : public BaseMigration<MigrationBackupCodes> {
public:
    static std::vector<std::string> up() {
        SQLSchemaBuilder builder("backup_codes");
        std::vector<std::string> queries;

        queries.push_back(
            builder.AddColumn("code TEXT NULL")
            .AddColumn("is_used BOOLEAN DEFAULT FALSE")
            .AddColumn("user_id INT NOT NULL")
            .CreateTable()
        );
			
        queries.push_back(
            builder.AddForeignKey("user_id", "users", "id")
            .AddIndex("idx_user_id_code", { "user_id", "code" })
        );

        return queries;
    }

    static std::string down() {
        SQLSchemaBuilder builder("users");
        return builder.DropTable();
    }
};