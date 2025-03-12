#pragma once
#include <string>
#include "../SQLSchemaBuilder.hpp"

template <typename Derived>
class BaseMigration {


public: 
	virtual ~BaseMigration() = default;

	static std::vector<std::string> up() {
		return Derived::up();
	}

	static std::string down() {
		return Derived::down();
	}
};