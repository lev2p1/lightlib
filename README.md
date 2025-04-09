# Light

Этот проект является MVC фреймворком для веб-разработки.

## Требования

Для успешной сборки и запуска проекта необходимо установить следующие зависимости:

### 1. **PostgreSQL**
   - Установите PostgreSQL версии 17 или выше.
   - Убедитесь, что библиотека **libpq** доступна в вашей системе.
   - На Linux установите пакет `libpq-dev`:
     ```bash
     sudo apt-get install libpq-dev
     ```
   - На Windows библиотека **libpq** поставляется вместе с PostgreSQL. Убедитесь, что пути к `include` и `lib` указаны в настройках проекта.

### 2. **Boost**
   - Установите библиотеку Boost версии 1.75 или выше.
   - На Linux установите пакет `libboost-all-dev`:
     ```bash
     sudo apt-get install libboost-all-dev
     ```
   - На Windows скачайте Boost с [официального сайта](https://www.boost.org/) и настройте пути в вашем проекте.

### 3. **Компилятор C++**
   - Поддерживается компилятор с поддержкой стандарта C++17 или выше (например, GCC, Clang, MSVC).

---

## Сборка проекта

### На Linux (с использованием CMake)
1. Убедитесь, что установлены все зависимости:
   ```bash
   sudo apt-get install libpq-dev libboost-all-dev cmake build-essential
   ```

## Использование

Этот раздел описывает, как использовать проект для работы с базой данных PostgreSQL и библиотекой Boost.

1. Подключение к базе данных
Для подключения к базе данных PostgreSQL используйте следующие параметры:

    - Хост: 127.0.0.1

    - Пользователь: postgres

    - Пароль: your_password

    - База данных: your_database

```bash
auto db = std::make_shared<Database>("127.0.0.1", "postgres", "your_password", "your_database");
```
2. Создание таблицы
```bash
db->execute("CREATE TABLE IF NOT EXISTS users (id SERIAL PRIMARY KEY, name VARCHAR(255), age INT);");
```
3. Вставка данных
```bash
db->execute("INSERT INTO users (name, age) VALUES ('Alice', 25);");
db->execute("INSERT INTO users (name, age) VALUES ('Bob', 30);");
```
4. Запрос данных
```bash
std::string result = db->query("SELECT * FROM users");
std::cout << result << std::endl;
```  
## Система логирования
При первом запуске вашего приложения система создает один и единственный лог-файл по пути Storage/Debug/debug.log. Чтобы инициализировать логер для сохранения важных для вас сообщений от приложения необходимо сделать следующее:
### 1. Подключение заголовчного файла логера
```bash
#include "../vendor/Debug/Logger.hpp"
``` 
### 2. Использование логера в вашем коде
```bash
Logger::log("Текст события", "Тип события");
```
## Работа с кэшами
Фреймворк предоставляет удобный класс Cache для работы с кэшами на основе Redis. Этот класс позволяет вам сохранять данные в кэше, извлекать их, управлять временем жизни ключей и удалять данные, когда они больше не нужны.
Подключение к Redis

Перед началом работы с кэшем необходимо подключиться к Redis. Для этого используется метод connect:
```bash
Cache::connect("127.0.0.1", 6379); // Подключение к Redis на localhost, порт 6379
```
Параметры:

   - host (по умолчанию "127.0.0.1") — адрес сервера Redis.

   - port (по умолчанию 6379) — порт сервера Redis.

Исключения:

   - Если подключение уже установлено, будет выброшено исключение runtime_error.

   - Если подключение не удалось, будет выброшено исключение с описанием ошибки.

### 1. Сохранение данных в кэше

Для сохранения данных в кэше используется метод set. 
Вы можете указать ключ, значение и время его жизни (опционально):
```bash
Cache::set("user:123", "John Doe"); // Сохранить значение "John Doe" по ключу "user:123"
Cache::set("session:456", "active", 60); // Сохранить значение "active" по ключу "session:456" с временем жизни 60 секунд
```
Параметры:

   - key — уникальный ключ, по которому будет сохранено значение.

   - value — данные, которые нужно сохранить.

   - expire_seconds (опционально) — время жизни ключа в секундах. Если не указано, ключ будет храниться бессрочно.

Исключения:

   - Если подключение к Redis не установлено, будет выброшено исключение std::runtime_error.

### 2. Получение данных из кэша

Для извлечения данных из кэша используется метод get:
```bash
std::string user = Cache::get("user:123"); // Получить значение по ключу "user:123"
if (!user.empty()) {
    std::cout << "User: " << user << std::endl;
} else {
    std::cout << "Key not found." << std::endl;
}
```
Возвращаемое значение:

   - Если ключ существует, возвращается его значение.

   - Если ключ не найден, возвращается пустая строка.

Исключения:

   - Если подключение к Redis не установлено, будет выброшено исключение std::runtime_error.

### 3. Удаление данных из кэша

Для удаления данных из кэша используется метод del:

```bash
Cache::del("user:123"); // Удалить ключ "user:123" и его значение
```
Параметры:

   - key — ключ, который нужно удалить.

Исключения:

   - Если подключение к Redis не установлено, будет выброшено исключение std::runtime_error.

### 4. Установка времени жизни ключа

Если вы хотите установить или изменить время жизни ключа, используйте метод expire:
```bash
Cache::expire("session:456", 120); // Установить время жизни ключа "session:456" на 120 секунд
```
Параметры:

   - key — ключ, для которого нужно установить время жизни.

   - expire_seconds — время жизни ключа в секундах.

Исключения:

   - Если подключение к Redis не установлено, будет выброшено исключение std::runtime_error.

### 5. Отключение от Redis

После завершения работы с кэшем рекомендуется отключиться от Redis с помощью метода disconnect:
```bash
Cache::disconnect(); // Отключение от Redis
```

## Работа с маршрутами

Динамические маршруты позволяют вам извлекать параметры из URL. Например, маршрут /users/{id} может извлечь значение id из URL /users/123.

### Синтаксис:

   - Динамические параметры указываются в фигурных скобках: {параметр}.

   - Например: /users/{id}, /storage/{path}.

### Пример добавления маршрута:

```bash
Router::get("/users/{id}", [](const Router::Request& req, Router::Response& res, const std::unordered_map<std::string, std::string>& params) {
    std::string userId = params.at("id"); // Извлекаем параметр "id"
    res.body() = "User ID: " + userId;
    res.result(http::status::ok);
});
```
### Обработка параметров

Параметры из динамических маршрутов передаются в обработчик в виде **std::unordered_map<std::string, std::string>**. 

Ключом является имя параметра (например, id), а значением — соответствующая часть URL.

**Пример обработки параметров**
```bash
Router::get("/storage/{path}", [](const Router::Request& req, Router::Response& res, const std::unordered_map<std::string, std::string>& params) {
    std::string path = params.at("path"); // Извлекаем параметр "path"
    res.body() = "Requested path: " + path;
    res.result(http::status::ok);
});
```

### Примеры использования

**Пример 1: Получение данных пользователя**
```bash
Router::get("/users/{id}", [](const Router::Request& req, Router::Response& res, const std::unordered_map<std::string, std::string>& params) {
    std::string userId = params.at("id");
    res.body() = "Fetching data for user ID: " + userId;
    res.result(http::status::ok);
});
```
   - URL: /users/123

   - Результат: Fetching data for user ID: 123

**Пример 2: Получение файла из хранилища**
```bash
Router::get("/storage/{path}", [](const Router::Request& req, Router::Response& res, const std::unordered_map<std::string, std::string>& params) {
    std::string path = params.at("path");
    res.body() = "Requested file: " + path;
    res.result(http::status::ok);
});
```
   - URL: /storage/images/photo.jpg

   - Результат: Requested file: images/photo.jpg

**Пример 3: Маршрут с несколькими параметрами**
```bash
Router::get("/posts/{category}/{id}", [](const Router::Request& req, Router::Response& res, const std::unordered_map<std::string, std::string>& params) {
    std::string category = params.at("category");
    std::string postId = params.at("id");
    res.body() = "Category: " + category + ", Post ID: " + postId;
    res.result(http::status::ok);
});
```
   - URL: /posts/tech/456

   - Результат: Category: tech, Post ID: 456

### Обработка статических маршрутов

Если вам не нужны параметры, вы можете использовать статические маршруты. Они работают так же, как и динамические, но без извлечения параметров.

**Пример статического маршрута**
```bash
Router::get("/about", [](const Router::Request& req, Router::Response& res) {
    res.body() = "About page";
    res.result(http::status::ok);
});
```
   - URL: /about

   - Результат: About page

### Обработка ошибок
   - Если маршрут не найден, фреймворк автоматически возвращает ответ с кодом 404 Not Found.

**Пример ошибки**
```bash
// Попытка доступа к несуществующему маршруту
req.method(http::verb::get);
req.target("/unknown");
Router::handle_request(req, res);

std::cout << res.body() << std::endl; // Вывод: "Error 404: Page not found.
```


## Система миграции 

Фреймворк предоставляет удобный интерфейс для работы с миграциями.

### Структра
 - Database.hpp: Класс для работы с базой данных.

 - SQLSchemaBuilder.hpp: Класс для построения SQL-запросов.

 - MigrationManager.hpp: Класс для управления миграциями.

 - migrations/: Директория с миграциями (например, migration_users_create.hpp).

### Использование

Миграция — это класс, который наследуется от BaseMigration и реализует методы up и down.

**Пример миграции для создания таблицы users:**

```bash
#pragma once

#include "BaseMigration.hpp"
#include "SQLSchemaBuilder.hpp"

class MigrationUsersCreate : public BaseMigration<MigrationUsersCreate> {
public:
    static std::vector<std::string> up() {
        SQLSchemaBuilder builder("users");
        std::vector<std::string> queries;

        // Основной запрос на создание таблицы
        queries.push_back(builder
            .AddColumn("id SERIAL PRIMARY KEY")
            .AddColumn("username VARCHAR(255) NOT NULL")
            .AddColumn("email VARCHAR(255) NOT NULL")
            .CreateTable());

        // Дополнительные запросы (индексы и уникальные ограничения)
        queries.push_back(builder.AddUniqueConstraint("uq_email", {"email"}));
        queries.push_back(builder.AddIndex("idx_username", {"username"}));

        return queries;
    }

    static std::string down() {
        SQLSchemaBuilder builder("users");
        return builder.DropTable();
    }
};
```

### Выполнение миграций
Для выполнения миграций используйте класс MigrationManager. **Пример:**

```bash
#include "MigrationManager.hpp"
#include "Database.hpp"
#include "migrations/migration_users_create.hpp"

int main() {
    Database db("your_database_config_here");
    MigrationManager manager(db);

    try {
        manager.migrateAll<
            MigrationUsersCreate
        >();
        std::cout << "Migrations executed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error executing migrations: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```

### Откат миграций
Для отката миграций используйте метод rollbackAll:

```bash
manager.rollbackAll<
    MigrationUsersCreate
>();
```
**Примеры**
Пример миграции для создания таблицы posts

```bash
#pragma once

#include "BaseMigration.hpp"
#include "SQLSchemaBuilder.hpp"

class MigrationPostsCreate : public BaseMigration<MigrationPostsCreate> {
public:
    static std::vector<std::string> up() {
        SQLSchemaBuilder builder("posts");
        std::vector<std::string> queries;

        queries.push_back(builder
            .AddColumn("id SERIAL PRIMARY KEY")
            .AddColumn("title VARCHAR(255) NOT NULL")
            .AddColumn("content TEXT NOT NULL")
            .AddColumn("user_id INT REFERENCES users(id)")
            .CreateTable());

        queries.push_back(builder.AddIndex("idx_title", {"title"}));

        return queries;
    }

    static std::string down() {
        SQLSchemaBuilder builder("posts");
        return builder.DropTable();
    }
};
```

### API

**SQLSchemaBuilder**
Класс для построения SQL-запросов.

**Основные методы:**
 - AddColumn: Добавляет столбец.

 - AddPrimaryKey: Добавляет первичный ключ.

 - AddForeignKey: Добавляет внешний ключ.

 - AddIndex: Добавляет индекс.

 - AddUniqueConstraint: Добавляет уникальное ограничение.

 - CreateTable: Возвращает SQL-запрос для создания таблицы.

 - DropTable: Возвращает SQL-запрос для удаления таблицы.

**MigrationManager**
Класс для управления миграциями.

**Основные методы:**
 - migrate: Выполняет одну миграцию.

 - rollback: Откатывает одну миграцию.

 - migrateAll: Выполняет несколько миграций.

 - rollbackAll: Откатывает несколько миграций.

### Логирование и отладка
Для отладки SQL-запросов добавлено логирование. Перед выполнением каждого запроса он выводится в консоль. **Пример вывода:**

```
[SQL Query] CREATE TABLE users (id SERIAL PRIMARY KEY, username VARCHAR(255) NOT NULL, email VARCHAR(255) NOT NULL);
[SQL Query] CREATE INDEX idx_username ON users (username);
[SQL Query] ALTER TABLE users ADD CONSTRAINT uq_email UNIQUE (email);
```

### Добавление миграций в очередь

Для добавления миграции в очередь требуется перейти в функцию **Initialize** и прописать через запятую какие миграции должны быть выполнены.

```bash
void Initialize() {
    this->migrateAll<
        MigrationUsersCreate
    >();
}
```
## Конструктор SQL-запросов

**SQLBuilder**

Класс SQLQueryBuilder предназначен для построения SQL-запросов, которые выполняют операции с данными в таблицах.

**Основные методы:**
- Выборки данных (SELECT).

- Вставки данных (INSERT).

- Обновления данных (UPDATE).

- Удаления данных (DELETE).

- Добавления условий (WHERE).

- Сортировки (ORDER BY).

- Группировки (GROUP BY).

- Ограничения количества строк (LIMIT, OFFSET).

- Соединения таблиц (JOIN).

**Отладка**

Метод get() в SQLQueryBuilder отвечает за формирование итогового SQL-запроса на основе всех настроек, которые были заданы с помощью других методов. **Пример использования:**

```bash
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
                .Where("users.name LIKE '%John%'")
                .OrderBy({ "users.name" })
                .Limit(10)
                .get();
```
**Результат выполнения запроса**

```bash
SELECT users.id, users.name, orders.order_id
FROM users
LEFT JOIN orders ON users.id = orders.user_id
WHERE (users.age > 18 AND (users.status = 'active' OR orders.total > 100)) AND users.name LIKE '%John%'
ORDER BY users.name
LIMIT 10
```


