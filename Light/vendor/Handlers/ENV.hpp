#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stdexcept>

class ENV {
public:
    static inline std::unordered_map<std::string, std::string> env_variables;
    static bool initialized;

    // Путь к .env файлу (на два уровня выше)
    static const std::string env_file_path;

    static void initialize() {
        if (initialized) return;

        std::ifstream env_file(env_file_path);
        if (!env_file.is_open()) {
            throw std::runtime_error("Could not open .env file at path: " + env_file_path);
        }

        std::string line;
        while (std::getline(env_file, line)) {
            // Убираем пробелы в начале и конце строки
            line.erase(0, line.find_first_not_of(' '));
            line.erase(line.find_last_not_of(' ') + 1);

            // Пропускаем пустые строки и комментарии
            if (line.empty() || line[0] == '#') {
                continue;
            }

            size_t delimiter_pos = line.find('=');
            if (delimiter_pos != std::string::npos) {
                std::string key = line.substr(0, delimiter_pos);
                std::string value = line.substr(delimiter_pos + 1);

                // Убираем пробелы вокруг ключа и значения
                key.erase(0, key.find_first_not_of(' '));
                key.erase(key.find_last_not_of(' ') + 1);
                value.erase(0, value.find_first_not_of(' '));
                value.erase(value.find_last_not_of(' ') + 1);

                env_variables[key] = value;
            }
        }

        env_file.close();
        initialized = true;
    }

    static std::string get(const std::string& key) {
        initialize();
        if (env_variables.find(key) != env_variables.end()) {
            return env_variables[key];
        }
        throw std::runtime_error("Environment variable not found: " + key);
    }
};