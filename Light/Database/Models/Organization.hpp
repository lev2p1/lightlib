#pragma once
#include "Model.hpp"

class Organization : public Model<Organization> {
public:
    // �������������� ����������� ����������
    static inline std::string table_name = "organizations";
    static inline std::vector<std::string> fillable = { "name", "image", "id_user" }; // ������ fillable �����
    static inline std::vector<std::string> fields = { "id", "id_user", "name", "image" }; // ������ ���� �����
    std::map<std::string, std::string> attributes;
};