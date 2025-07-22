#pragma once
#include <nlohmann/json.hpp>
#include "Controller.hpp"
#include "../../../vendor/Facades/Hash.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
using json = nlohmann::json;

class HashController : public Controller {
	inline void show(const Request& req, Response& res, Params params) override {
		try {
			json body = json::parse(req.body());
			std::string userString = body["string"];

			auto hashPair = Hash::hash(userString, Hash::generateSalt(64));
			std::string userHasedString = hashPair.first;
			std::string userSalt = Hash::bytesToHexString(hashPair.second);

			json response = {
				{"user_string_hash", userHasedString},
				{"user_string_salt", userSalt}
			};

			res.result(http::status::ok);
			res.body() = response.dump();
		}
		catch (std::exception& e) {
			Logger::log(e.what(), "ERROR");
			res.result(http::status::bad_request);
		}
		
	}
};
