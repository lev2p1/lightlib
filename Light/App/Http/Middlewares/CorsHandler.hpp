#pragma once
#include <vector>
#include <string>
#include "Middleware.hpp"

class CorsHandler : public Middleware {
public:
	std::vector<std::string> allwed_domains;
	virtual ~CorsHandler() = default;
};