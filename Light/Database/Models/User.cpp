#include "Model.hpp"

class User : public Model<User> {
public:
    // Переопределяем статическую переменную
    static inline std::string table_name = "users";

    std::string getAttribute(const std::string& key) const override {
        return "123";
    };
    void setAttribute(const std::string& key, const std::string& value) override {
        
    }

    void load(int id) override {

    }
    void save() override {

    }
    bool validate() const override {
        return true;
    }


    std::string toJson() const override {
        return "123";
    };
    void fromJson(const std::string& json) override{
        
    }

    std::vector<std::string> getFieldNames() const override {
        return { "123" };
    }
    std::string getFieldType(const std::string& fieldName) const override {
        return "huy";
    }

};