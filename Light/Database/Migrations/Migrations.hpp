#pragma once 
#pragma execution_character_set("utf-8")

#include <vector>
#include <utility>
#include <functional>

class Migration {
	using Handler = std::function<void()>;
	static std::vector<std::pair<Handler, bool>> migrations_;

public:
	
	static void addMigration(Handler handle) {
		migrations_.push_back({ handle, false }); // ���������� false ������ 0
	}

	static void runMigrations() {
			for (auto& i : migrations_) {
				if (!i.second) {
					i.first(); // �������� �������
					i.second = true; // �������� �������� ��� �����������
				}
			}
		
	}

};