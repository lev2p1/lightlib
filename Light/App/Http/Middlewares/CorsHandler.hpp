#pragma once
#include <vector>
#include <string>
#include "Middleware.hpp"
#include <set>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <unordered_set>

class CorsHandler : public Middleware {
public:
	std::unordered_set<std::string> allowed_domains;
	std::vector<std::string> allowed_methods;
	std::vector<std::string> allowed_headers;
	std::string allow_origin = "*";

	CorsHandler() = default;
	CorsHandler(const std::vector<std::string>& domains,
				const std::vector<std::string>& methods = {"GET", "POST", "OPTIONS"},
				const std::vector<std::string>& headers = {"Content-Type", "Authorization"})
		: allowed_domains(domains.begin(), domains.end()), allowed_methods(methods), allowed_headers(headers) {}

	void handle(Request& req, Response& res, std::function<void()> next) override {
		std::string origin = std::string(req[http::field::origin]);
		bool allowed = allowed_domains.empty() ||
			allowed_domains.find(origin) != allowed_domains.end() ||
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
		if (v.empty()) return "";
		std::ostringstream oss;
		std::copy(v.begin(), v.end() - 1, std::ostream_iterator<std::string>(oss, delim.c_str()));
		oss << v.back();
		return oss.str();
	}

	virtual ~CorsHandler() = default;
};