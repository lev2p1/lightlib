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
### Система логирования
Логгер является статическим классом и хранит логи по пути Storage/Debug/debug.log  