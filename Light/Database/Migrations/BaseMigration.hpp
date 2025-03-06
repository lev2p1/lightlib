#pragma once
#include <string>
#include "../SQLSchemaBuilder.hpp"

template <typename Derived>
class BaseMigration {


public: 
	virtual ~BaseMigration() = default;

	static std::string up() {
		return Derived::up();
	}
};