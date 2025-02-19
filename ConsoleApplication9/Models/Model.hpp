#pragma once
#include <iostream>
#include <map>
#include <vector>

class Model {

protected:
	std::string table_name;
	std::map<std::string, std::string> result;
	std::vector<std::string> fillable;

public:
	Model* create(std::map<std::string, std::string> result);

	~Model() = default;
};