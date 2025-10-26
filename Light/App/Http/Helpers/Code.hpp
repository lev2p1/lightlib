#pragma once
#include <map>
#include <string>
#include <sstream>
#include <boost/beast/http.hpp>

class Code {
public:
	static inline std::string generateRandomCode(size_t length) {
		const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
		std::string code;
		code.reserve(length);
		for (size_t i = 0; i < length; ++i) {
			code += characters[rand() % characters.size()];
		}
		return code;
	}

	static inline std::string generateNumericCode(size_t length) {
		const std::string characters = "0123456789";
		std::string code;
		code.reserve(length);
		for (size_t i = 0; i < length; ++i) {
			code += characters[rand() % characters.size()];
		}
		return code;
	}

	static inline std::vector<std::string> generateMultipleCodes(size_t count, size_t length, bool numeric = false) {
		std::vector<std::string> codes;
		codes.reserve(count);
		for (size_t i = 0; i < count; ++i) {
			if (numeric) {
				codes.push_back(generateNumericCode(length));
			} else {
				codes.push_back(generateRandomCode(length));
			}
		}
		return codes;
	}
};