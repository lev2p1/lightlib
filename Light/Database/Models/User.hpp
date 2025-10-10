#pragma once
#include "Model.hpp"

class User : public Model<User> {
public:
    static inline std::string table_name = "users";
    static inline std::vector<std::string> fillable = {"username", "email", "password", "salt"};
    static inline std::vector<std::string> fields = { "id", "username", "email", "password", "salt"};
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
};