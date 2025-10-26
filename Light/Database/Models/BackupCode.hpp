#pragma once
#include "Model.hpp"
#include "User.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class BackupCode : public Model<BackupCode> {
public:
    static inline std::string table_name = "backup_codes";

    static inline std::vector<std::string> fillable = {
        "code",
        "is_used",
        "user_id"
    };

    static inline std::vector<std::string> fields = {
        "code",
        "is_used",
        "user_id"
    };

    std::map<std::string, std::string> attributes;

    static inline std::vector<std::string> getBackupCodesByUserId(int userId) {
        std::vector<std::string> codes;
        try {
			auto results = User::where("user_id = " + std::to_string(userId));
            for (const auto& record : results) {
                codes.push_back(record->getAttribute("code"));
            }
        }
        catch (const std::exception& e) {
            Logger::log("Error retrieving backup codes: " + std::string(e.what()), "ERROR");
        }
        return codes;
	}

    static inline bool deleteBackupCodesByUserId(int userId) {
        try {
            return BackupCode::deleteWhere("user_id = " + std::to_string(userId));
        }
        catch (const std::exception& e) {
            Logger::log("Error deleting backup codes: " + std::string(e.what()), "ERROR");
        }
    }

    static inline bool createBackupCodes(int userId, const std::vector<std::string>& codes) {
        try {
			std::vector<std::shared_ptr<BackupCode>> backupCodes;
            for (const auto& code : codes) {
                auto backupCode = BackupCode::create({
                    {"code", code},
                    {"is_used", "0"},
                    {"user_id", std::to_string(userId)}
                    });
				backupCodes.push_back(backupCode);
            }
            BackupCode::deleteBackupCodesByUserId(userId);
			BackupCode::saveMany(backupCodes);
            return true;
        }
        catch (const std::exception& e) {
            Logger::log("Error creating backup codes: " + std::string(e.what()), "ERROR");
            return false;
        }
    }
};