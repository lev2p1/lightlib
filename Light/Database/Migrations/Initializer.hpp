#pragma once

#include "Migrations.hpp"
#include "migration_posts_create.hpp"


class Initializer {
public:

    static void initMigrations() {
        Migration::addMigration([]() {
            std::string result = MigrationPostsCreate::up();
            std::cout << "Migration result: " << result << std::endl;
            });

        std::cout << "Inited" << std::endl;

        Migration::runMigrations();
    }
};