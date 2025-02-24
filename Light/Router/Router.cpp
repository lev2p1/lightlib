#include "Router.hpp"

std::unordered_map<http::verb, std::unordered_map<std::string, Router::Handler>> Router::routes_;