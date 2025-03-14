#pragma once
#pragma warning(disable : 4996)

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <filesystem>
#include <csignal>
#include <cstdlib>

class Logger {
private:
    static std::ofstream logFile; // Поток для записи в лог-файл
    static std::mutex logMutex; // Мьютекс для потокобезопасности
    static std::filesystem::path logFilePath; // Путь к лог-файлу
    static size_t maxLines; // Максимальное количество строк (10 000)
    static size_t currentLineCount; // Текущее количество строк

    // RAII-обёртка для сброса буферов
    struct FlushOnExit {
        ~FlushOnExit() {
            if (logFile.is_open()) {
                logFile.flush();
            }
        }
    };

    static FlushOnExit flushOnExit;

    // Статический метод для обработки сигналов
    static void signalHandler(int signal);

    // Метод для ротации лог-файлов
    static void rotateLogs();

public:
    Logger() = default;
    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Инициализация логгера
    static void init(const std::string& filename);

    // Логирование сообщений
    static void log(const std::string& message, const std::string& level = "INFO");

    // Метод для регистрации обработчиков сигналов
    static void registerSignalHandlers() {
        std::signal(SIGINT, signalHandler);  // Ctrl+C
        std::signal(SIGTERM, signalHandler); // Команда kill
    }
};


// Определение статических членов
inline std::ofstream Logger::logFile;
inline std::mutex Logger::logMutex;
inline std::filesystem::path Logger::logFilePath;
inline size_t Logger::maxLines = 10000; // Максимальное количество строк
inline size_t Logger::currentLineCount = 0; // Текущее количество строк
inline Logger::FlushOnExit Logger::flushOnExit;

// Метод для обработки сигналов
inline void Logger::signalHandler(int signal) {
    log("Program terminated with signal: " + std::to_string(signal), "ERROR");
    log("Application finished", "INFO");
    std::exit(signal); // Завершаем программу
}

// Метод для ротации лог-файлов
inline void Logger::rotateLogs() {
    if (logFile.is_open()) {
        logFile.close(); // Закрываем текущий файл
    }

    // Переименовываем текущий файл
    std::filesystem::path oldLogPath = logFilePath;

    std::string result = oldLogPath.filename().string(); // Копируем основную строку
    size_t pos = result.find(oldLogPath.extension().string()); // Ищем подстроку

    if (pos != std::string::npos) { // Если подстрока найдена
        result.erase(pos, oldLogPath.extension().string().length()); // Удаляем подстроку
    }

    oldLogPath.replace_filename(result + "_old" + oldLogPath.extension().string());

    // Удаляем старый файл, если он существует
    if (std::filesystem::exists(oldLogPath)) {
        std::filesystem::remove(oldLogPath);
    }

    // Переименовываем текущий файл в старый
    std::filesystem::rename(logFilePath, oldLogPath);

    // Создаем новый файл
    logFile.open(logFilePath, std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to open log file: " + logFilePath.string());
    }

    currentLineCount = 0; // Сбрасываем счетчик строк
}

// Инициализация логгера
inline void Logger::init(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex);

    // Получаем путь к папке Storage/Logs
    std::filesystem::path logDir = std::filesystem::current_path() / "Storage" / "Logs";

    // Создаем папку, если она не существует
    if (!std::filesystem::exists(logDir)) {
        std::filesystem::create_directories(logDir);
    }

    // Формируем полный путь к лог-файлу
    logFilePath = logDir / filename;

    // Открываем файл для записи
    logFile.open(logFilePath, std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to open log file: " + logFilePath.string());
    }

    // Инициализируем счетчик строк
    std::ifstream inFile(logFilePath);
    currentLineCount = std::count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n');
    inFile.close();
}

// Логирование сообщений
inline void Logger::log(const std::string& message, const std::string& level) {
    std::lock_guard<std::mutex> lock(logMutex);
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);

    if (logFile.is_open()) {
        logFile << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " [" << level << "] " << message << std::endl;
        logFile.flush(); // Сбрасываем буфер на диск

        // Увеличиваем счетчик строк
        currentLineCount++;

        // Проверяем, нужно ли выполнить ротацию
        if (currentLineCount >= maxLines) {
            rotateLogs();
        }
    }
    std::cout << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " [" << level << "] " << message << std::endl;
}