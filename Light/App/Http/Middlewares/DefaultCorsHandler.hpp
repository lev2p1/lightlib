#include "CorsHandler.hpp"

class DefaultCorsHandler : public CorsHandler {
	inline bool handle(Request& req) {
		return true;
	}
};