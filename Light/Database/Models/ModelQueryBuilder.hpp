#pragma once
#include "../SQLBuilder.hpp"
#include "../Database.hpp"
#include <vector>
#include <memory>

template<typename Derived>
class ModelQueryBuilder : public SQLQueryBuilder {
public:
    using SQLQueryBuilder::SQLQueryBuilder;

    ModelQueryBuilder& Select(const std::vector<std::string>& columns) {
        SQLQueryBuilder::Select(columns);
        return *this;
    }
    ModelQueryBuilder& Where(const std::string& condition, const std::string& logicalOperator = "AND") {
        SQLQueryBuilder::Where(condition, logicalOperator);
        return *this;
    }
    ModelQueryBuilder& OrderBy(const std::vector<std::string>& columns) {
        SQLQueryBuilder::OrderBy(columns);
        return *this;
    }
    ModelQueryBuilder& Limit(int value) {
        SQLQueryBuilder::Limit(value);
        return *this;
    }
    ModelQueryBuilder& Offset(int value) {
        SQLQueryBuilder::Offset(value);
        return *this;
    }
    // ...добавьте другие методы по необходимости

    std::vector<std::shared_ptr<Derived>> get() {
        auto database = std::make_shared<Database>();
        auto rows = database->queryToVector(this->getQuery());
        std::vector<std::shared_ptr<Derived>> result;
        for (const auto& row : rows) {
            auto model = Derived::create(row);
            if (model) result.push_back(model);
        }
        return result;
    }
private:
    std::string getQuery() {
        return SQLQueryBuilder::get();
    }
}; 