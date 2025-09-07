#include <string>
#include <cctype>
#include <regex>

class Validator {
public:
    static inline bool password(const std::string& password) {
        if (password.size() < 8) return false;

        bool hasDigit = false;
        bool hasUpper = false;
        bool hasSpecial = false;

        for (char ch : password) {
            if (std::isdigit(ch)) hasDigit = true;
            else if (std::isupper(ch)) hasUpper = true;
            else if (!std::isalnum(ch)) hasSpecial = true;
        }

        return hasDigit && hasUpper && hasSpecial;
    }


    static inline bool email(const std::string& email) {
        const std::regex pattern(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
        return std::regex_match(email, pattern);
    }

};