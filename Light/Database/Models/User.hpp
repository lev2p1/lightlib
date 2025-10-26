#pragma once
#include "Model.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class User : public Model<User> {
public:
    static inline std::string table_name = "users";

    static inline std::vector<std::string> fillable = {
        "username",
        "email",
        "password",
        "salt",
        "picture",
        "is_verified",
        "verification_token",
        "two_fa_enabled",
        "two_fa_secret",
        "backup_codes",
        "last_login",
        "role",
        "status",
        "last_password_change",
        "reset_token",
        "reset_token_expires_at",
        "timezone",
        "language",
        "country"
    };

    static inline std::vector<std::string> fields = {
        "id",
        "username",
        "email",
        "password",
        "salt",
        "picture",
        "is_verified",
        "verification_token",
        "two_fa_enabled",
        "two_fa_secret",
        "backup_codes",
        "last_login",
        "role",
        "status",
        "last_password_change",
        "reset_token",
        "reset_token_expires_at",
        "timezone",
        "language",
        "country",
        "created_at",
        "updated_at",
        "deleted_at"
    };

    std::map<std::string, std::string> attributes;

    static std::shared_ptr<User> findByUsername(std::string username){
        auto users = User::where("username = '" + username + "'");

        if(users.size() > 0){
            return users[0];
        }
        else{
            return nullptr;
        }
    }

    static std::shared_ptr<User> findByEmail(std::string email){
        auto users = User::where("email = '" + email + "'");

        if(users.size() > 0){
            return users[0];
        }
        else{
            return nullptr;
        }
    }

    json toJson() {
        json j;
        for (const auto& field : fields) {
            j[field] = this->getAttribute(field);
        }
        return j;
	}
};