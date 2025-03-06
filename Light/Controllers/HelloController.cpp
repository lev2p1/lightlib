#include "HelloController.hpp"
#include "../Database/SQLBuilder.hpp"


using json = nlohmann::json;

void HelloController::handle(const Request& req, Response& res)
{
	res.result(http::status::ok);
	res.body() = "Hello from handle function";
}

void HelloController::index(const Request& req, Response& res)
{
	res.result(http::status::ok);
	res.body() = "Hello from index function";
}

void HelloController::store(const Request& req, Response& res) {
    try {
        auto db = std::make_shared<Database>();

        // Тесты для SQLQueryBuilder
        std::cout << "Running SQLQueryBuilder tests...\n";

        // Тест 1: Простой запрос с одним условием
        {
            SQLQueryBuilder builder("users");
            std::string query = builder
                .Select({ "id", "name" })
                .Where("age > 18")
                .get();
            std::cout << "Test 1 - Simple Query:\n" << query << "\n\n";
        }

        // Тест 2: Запрос с несколькими условиями (AND)
        {
            SQLQueryBuilder builder("users");
            std::string query = builder
                .Select({ "id", "name", "age" })
                .Where("age > 18")
                .Where("status = 'active'")
                .Where("name LIKE '%kolyan%'")
                .get();
            std::cout << "Test 2 - Multiple Conditions (AND):\n" << query << "\n\n";
        }

        // Тест 3: Запрос с условиями OR
        {
            SQLQueryBuilder builder("users");
            std::string query = builder
                .Select({ "id", "name" })
                .Where("age > 18")
                .Where("status = 'active'", "OR")
                .get();
            std::cout << "Test 3 - OR Conditions:\n" << query << "\n\n";
        }

        // Тест 4: Запрос с группировкой условий
        {
            SQLQueryBuilder builder("users");
            std::string query = builder
                .Select({ "id", "name", "age" })
                .BeginGroup()
                .Where("age > 18")
                .Where("password = 'sha-0001'", "OR")
                .EndGroup()
                .Where("name LIKE '%kolyan%'")
                .get();
            std::cout << "Test 4 - Grouped Conditions:\n" << query << "\n\n";
        }

        // Тест 5: Вложенные группы условий
        {
            SQLQueryBuilder builder("users");
            std::string query = builder
                .Select({ "id", "name", "age" })
                .BeginGroup()
                .Where("age > 18")
                .BeginGroup("OR")
                .Where("password = 'sha-0001'")
                .Where("status = 'active'")
                .EndGroup()
                .EndGroup()
                .Where("name LIKE '%kolyan%'")
                .get();
            std::cout << "Test 5 - Nested Groups:\n" << query << "\n\n";
        }

        // Тест 6: Запрос с JOIN
        {
            SQLQueryBuilder builder("users");
            std::string query = builder
                .Select({ "users.id", "users.name", "orders.order_id" })
                .Join("orders", "users.id = orders.user_id", "LEFT")
                .Where("users.age > 18")
                .get();
            std::cout << "Test 6 - JOIN:\n" << query << "\n\n";
        }

        // Тест 7: Запрос с GROUP BY и ORDER BY
        {
            SQLQueryBuilder builder("users");
            std::string query = builder
                .Select({ "age", "COUNT(*) as user_count" })
                .GroupBy({ "age" })
                .OrderBy({ "user_count DESC" })
                .get();
            std::cout << "Test 7 - GROUP BY and ORDER BY:\n" << query << "\n\n";
        }

        // Тест 8: Запрос с LIMIT и OFFSET
        {
            SQLQueryBuilder builder("users");
            std::string query = builder
                .Select({ "id", "name" })
                .Where("age > 18")
                .Limit(10)
                .Offset(5)
                .get();
            std::cout << "Test 8 - LIMIT and OFFSET:\n" << query << "\n\n";
        }

        // Тест 9: Запрос с INSERT
        {
            SQLQueryBuilder builder("users");
            std::string query = builder
                .Insert({
                    { "name", "'kolyan'" },
                    { "age", "20" },
                    { "password", "'sha-0001'" }
                    })
                .get();
            std::cout << "Test 9 - INSERT:\n" << query << "\n\n";
        }

        // Тест 10: Сложный запрос с вложенными группами и JOIN
        {
            SQLQueryBuilder builder("users");
            std::string query = builder
                .Select({ "users.id", "users.name", "orders.order_id" })
                .Join("orders", "users.id = orders.user_id", "LEFT")
                .BeginGroup()
                .Where("users.age > 18")
                .BeginGroup("OR")
                .Where("users.status = 'active'")
                .Where("orders.total > 100")
                .EndGroup()
                .EndGroup()
                .Where("users.name LIKE '%kolyan%'")
                .OrderBy({ "users.name" })
                .Limit(10)
                .get();
            std::cout << "Test 10 - Complex Query:\n" << query << "\n\n";
        }

        // Основной запрос
        SQLQueryBuilder builder("users");
        std::string query = builder
            .Select({ "id", "name", "age" })
            .BeginGroup() // Начало группы
            .Where("age > 18") // Первое условие в группе
            .Where("password = 'sha-0001'", "OR") // Второе условие в группе
            .EndGroup() // Конец группы
            .Where("name LIKE '%kolyan%'") // Условие вне группы
            .get();

        // Выводим запрос для отладки
        std::cout << "Generated SQL Query:\n" << query << std::endl;

        // Устанавливаем статус ответа
        res.result(http::status::ok);
        res.body() = "Запрос выполнен успешно";
        std::cout << "Operations completed successfully!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        Logger::log("Error: " + std::string(e.what()), "ERROR");
    }
}
 
void HelloController::login(const Request& req, Response& res)
{
	try {
		if (req.body().empty()) {
			throw std::runtime_error("Request body is empty");
		}

		json body = json::parse(req.body());

		// Извлекаем логин и пароль
		std::string username = body["username"];
		std::string password = body["password"];

		std::string hashed_password = Hash::hash(password, 10);

		auto data = User::where("name = '" + username + "'");

		for (const auto& user : data) {
			user->printAttributes();
			std::cout << user->attributes["name"];
		}

		if (data[0]->getAttribute("password") == hashed_password) {
			auto token = AuthService::createAccessToken(data[0]->getAttribute("id"));
			res.body() = "Authorization: " + token;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		Logger::log("Error: " + std::string(e.what()), "ERROR");
	}
}

void HelloController::reg(const Request& req, Response& res)
{
	try {
		if (req.body().empty()) {
			throw std::runtime_error("Request body is empty");
		}

		json body = json::parse(req.body());

		// Извлекаем логин и пароль
		std::string username = body["username"];
		std::string password = body["password"];
		std::string age = body["age"];

		std::string hashed_password = Hash::hash(password, 10);

		User::create({ {"name", username}, {"age", age}, {"password", hashed_password} })->save();

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		Logger::log("Error: " + std::string(e.what()), "ERROR");
	}
}

void HelloController::getAttr(const Request& req, Response& res) {
	
	try {
		setlocale(LC_ALL, "ru");
		User::delete_(16);
		res.result(http::status::ok);
		res.body() = "Данные сохранены успешно";
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void HelloController::testQueue(const Request& req, Response& res) {
	try {

		Queue::push("kolyan_queue", "Kolyan kolyan");

		std::string popa = Queue::pop("kolyan_queue");
		std::cout << popa << std::endl;
		res.body() = popa;
		
	}
	catch (const std::exception& e) {
		Logger::log("Error: " + std::string(e.what()), "ERROR");
		std::cerr << "Error: " << e.what() << std::endl;
	}
}


