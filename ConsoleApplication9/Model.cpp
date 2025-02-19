#include "Model.hpp"

Model* Model::create(std::map<std::string, std::string> result){
	/*for (auto i : result) {
		std::cout << i.first << " ";
		std::cout << i.second << std::endl;
	}*/
	for (auto i : this->fillable) {
		std::cout << i << " ";
	}
	return this;
}