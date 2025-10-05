#pragma once
#include <map>
#include <string>
#include <sstream>
#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

class Cookie{
public:
    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;

    static inline std::map<std::string, std::string> parseCookies(const std::string& cookieHeader) {
		std::map<std::string, std::string> cookies;
		std::istringstream stream(cookieHeader);
		std::string pair;

		while (std::getline(stream, pair, ';')) {
			auto pos = pair.find('=');
			if (pos != std::string::npos) {
				std::string key = pair.substr(0, pos);
				std::string value = pair.substr(pos + 1);
				key.erase(0, key.find_first_not_of(" \t"));
				value.erase(0, value.find_first_not_of(" \t"));
				cookies[key] = value;
			}
		}

		return cookies;
	}

    static inline void set(Response &res, std::map<std::string, std::string> cookies){
        for (const auto& [name, value] : cookies) {
            res.insert(http::field::set_cookie, name + "=" + value + "; Path=/; HttpOnly; Secure; SameSite=None"); // HTTPS: add Secure; to cookie 
        }
    }
};