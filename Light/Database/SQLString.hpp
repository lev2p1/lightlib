#pragma once
#include <string>
#include <libpq-fe.h>

class SQLString {
public:
    // Экранирование строки через существующее подключение
    static std::string EscapeString(PGconn* conn, const std::string& input) {
        char* escaped = PQescapeLiteral(conn, input.c_str(), input.size());
        if (!escaped) {
            return "'ERROR: FAILED TO ESCAPE STRING'";
        }
        std::string result(escaped);
        PQfreemem(escaped);
        return result;
    }

    // Экранирование идентификатора через существующее подключение
    static std::string EscapeIdentifier(PGconn* conn, const std::string& input) {
        char* escaped = PQescapeIdentifier(conn, input.c_str(), input.size());
        if (!escaped) {
            return "\"ERROR: FAILED TO ESCAPE IDENTIFIER\"";
        }
        std::string result(escaped);
        PQfreemem(escaped);
        return result;
    }
};