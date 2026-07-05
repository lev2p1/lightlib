#pragma once

#include <vector>
#include <string>
#include <memory>
#include <nlohmann/json.hpp>

namespace lightlib {

	using json = nlohmann::json;

	template <typename ModelType>
	class Collection : public std::vector<std::shared_ptr<ModelType>>{
	public:
		using std::vector<std::shared_ptr<ModelType>>::vector;

		Collection() = default;

		Collection(const std::vector<std::shared_ptr<ModelType>>& vec)
			: std::vector<std::shared_ptr<ModelType>>(vec) {}

		Collection(std::vector<std::shared_ptr<ModelType>>&& vec)
			: std::vector<std::shared_ptr<ModelType>>(std::move(vec)) {}

		bool save() {
			if (this->empty()) {
				Logger::log("Collection is empty, nothing to save", "WARNING");
				return false;
			}
			return ModelType::saveMany(*this);
		}

		bool delete_() {
			if (this->empty()) {
				Logger::log("Collection is empty, nothing to delete", "WARNING");
				return false;
			}

			bool success = true;
			for (auto& item : *this) {
				try {
					item->delete_();
				}
				catch (const std::exception& e) {
					Logger::log("Failed to delete item: " + std::string(e.what()), "ERROR");
					success = false;
				}
			}
			return success;
		}

		json toJson() const {
			json j = json::array();
			for (const auto& item : *this) {
				j.push_back(item->toJson());
			}
			return j;
		}

		std::shared_ptr<ModelType> first() const {
			return this->empty() ? nullptr : this->front();
		}

		std::shared_ptr<ModelType> last() const {
			return this->empty() ? nullptr : this->back();
		}

		Collection<ModelType> filter(std::function<bool(std::shared_ptr<ModelType>)> predicate) const {
			Collection<ModelType> result;
			std::copy_if(this->begin(), this->end(), std::back_inserter(result), predicate);
			return result;
		}

		bool all(std::function<bool(std::shared_ptr<ModelType>)> predicate) const {
			return std::all_of(this->begin(), this->end(), predicate);
		}

		bool any(std::function<bool(std::shared_ptr<ModelType>)> predicate) const {
			return std::any_of(this->begin(), this->end(), predicate);
		}

		std::shared_ptr<ModelType> find(std::function<bool(std::shared_ptr<ModelType>)> predicate) const {
			auto it = std::find_if(this->begin(), this->end(), predicate);
			return it != this->end() ? *it : nullptr;
		}

		std::vector<std::shared_ptr<ModelType>> toVector() const {
			return *this;
		}

		operator std::vector<std::shared_ptr<ModelType>>() const {
			return *this;
		}

		void debugPrint() const {
			std::cout << "COLLECTION (" << this->size() << " items)\n";
			for (size_t i = 0; i < this->size(); ++i) {
				std::cout << "Item " << i << ":\n";
				(*this)[i]->debugPrintAttributes();
				std::cout << "---\n";
			}
		}
	};
}