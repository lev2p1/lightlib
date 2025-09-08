#pragma once
#include "Middleware.hpp"

class CorsMiddleware : public Middleware{

    std::vector<std::string> allowed_domains;
    std::string allow_origin;

public:
    bool handle(Request& req, Response& res){
        std::string origin = std::string(req[http::field::origin]);
		bool allowed = this->allowed_domains.empty() ||
			std::find(allowed_domains.begin(), allowed_domains.end(), origin) != allowed_domains.end();

		if (!allowed) {
			res.result(http::status::forbidden);
			res.body() = "CORS: Origin not allowed";
			res.set(http::field::access_control_allow_origin, "null");
			res.prepare_payload();
			return false;
		}

		this->setCors(req, res);

		if (req.method() == http::verb::options) {
			res.result(http::status::ok);
			res.body() = "CORS preflight";
			res.prepare_payload();
			return true;
		}
    }
};