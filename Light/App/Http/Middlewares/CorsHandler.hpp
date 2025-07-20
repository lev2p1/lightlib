#pragma once
#include <vector>
#include <string>
#include "Middleware.hpp"
#include <set>
#include <algorithm>

class CorsHandler : public Middleware {
public:
	std::vector<std::string> allowed_domains;
	std::vector<std::string> allowed_methods;
	std::vector<std::string> allowed_headers;
	std::string allow_origin = "*";

	CorsHandler() = default;
	CorsHandler(const std::vector<std::string>& domains,
				const std::vector<std::string>& methods = {"GET", "POST", "OPTIONS"},
				const std::vector<std::string>& headers = {"Content-Type", "Authorization"})
		: allowed_domains(domains), allowed_methods(methods), allowed_headers(headers) {}

	void handle(Request& req, Response& res, std::function<void()> next) override {
		std::string origin = std::string(req[http::field::origin]);
		bool allowed = allowed_domains.empty() ||
			std::find(allowed_domains.begin(), allowed_domains.end(), origin) != allowed_domains.end() ||
			allow_origin == "*";

		if (!allowed) {
			res.result(http::status::forbidden);
			res.body() = "CORS: Origin not allowed";
			res.set(http::field::access_control_allow_origin, "null");
			res.prepare_payload();
			return;
		}

		res.set(http::field::access_control_allow_origin, allow_origin == "*" ? "*" : origin);
		res.set(http::field::access_control_allow_methods, join(allowed_methods, ", "));
		res.set(http::field::access_control_allow_headers, join(allowed_headers, ", "));
		res.set(http::field::access_control_allow_credentials, "true");

		// OPTIONS preflight
		if (req.method() == http::verb::options) {
			res.result(http::status::ok);
			res.body() = "CORS preflight";
			res.prepare_payload();
			return;
		}
		next();
	}

	static std::string join(const std::vector<std::string>& v, const std::string& delim) {
		std::string out;
		for (size_t i = 0; i < v.size(); ++i) {
			out += v[i];
			if (i + 1 < v.size()) out += delim;
		}
		return out;
	}

	virtual ~CorsHandler() = default;
};