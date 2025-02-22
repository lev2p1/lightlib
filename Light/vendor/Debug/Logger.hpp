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
