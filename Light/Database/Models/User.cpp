#include "Model.hpp"

class User : public Model{
private:
    // Статическая переменная для хранения имени таблицы
    static inline std::string table_name = "Users";

public:
    
    // Атрибуты
    std::string getAttribute(const std::string& key) const override {
        return "1";
    }
    void setAttribute(const std::string& key, const std::string& value) override {

    }

    // Работа с данными
    void load(int id) override {

    }
    void save() override {

    }
    bool validate() const override {
        return true;
    }

    // Сериализация
    std::string toJson() const override {
        return "1";
    }
    void fromJson(const std::string& json) override {

    }

    // Метаданные
    std::vector<std::string> getFieldNames() const override {
        return { "1", "2" };
    }
    std::string getFieldType(const std::string& fieldName) const override {
        return "1";
    }
};