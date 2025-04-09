#pragma once
#include <string>
#include <regex>

class SQLString {
public:
    static std::string Escape(const std::string& input) {
        static const std::regex dangerousChars(R"(['"\\%;_])");
        return std::regex_replace(
            input,
            dangerousChars,
            R"(\\$&)",
            std::regex_constants::match_default |
            std::regex_constants::format_sed
        );
    }
};