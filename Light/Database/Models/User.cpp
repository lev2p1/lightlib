#pragma once
#include "Model.hpp"

class User : public Model<User> {
public:
    // �������������� ����������� ����������
    static inline std::string table_name = "users";
    static inline std::vector<std::string> fillable = {"username", "email", "password"}; // ������ fillable �����
    static inline std::vector<std::string> fields = { "id", "username", "email", "password"}; // ������ ���� �����
    std::map<std::string, std::string> attributes;
};