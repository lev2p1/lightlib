#include "Model.hpp"

class User : public Model{
public:
	User() {
		this->fillable = { "id", "name", "password"};
	}

	static void show() {
		std::cout << "User table";
	}
};