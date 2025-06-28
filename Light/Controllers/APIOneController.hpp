#pragma once
#include "APIController.hpp"

class APIOneController : public APIController {
	void show(Request req, Response& res);
	void update(Request req, Response& res);
	void delete_(Request req, Response& res);
	void store(Request req, Response& res);
};

inline void APIOneController::show(Request req, Response& res){

}

inline void APIOneController::update(Request req, Response& res)
{
}

inline void APIOneController::delete_(Request req, Response& res)
{
}

inline void APIOneController::store(Request req, Response& res)
{

}
