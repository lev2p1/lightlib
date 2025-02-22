#include "Logger.hpp"

// Определение статических членов
std::ofstream Logger::logFile;
std::mutex Logger::logMutex;
std::filesystem::path Logger::logFilePath;
size_t Logger::maxLines = 10; // Максимальное количество строк
size_t Logger::currentLineCount = 0; // Текущее количество строк
Logger::FlushOnExit Logger::flushOnExit;

// Метод для обработки сигналов
void Logger::signalHandler(int signal) {
    log("Program terminated with signal: " + std::to_string(signal), "ERROR");
    log("Application finished", "INFO");
    std::exit(signal); // Завершаем программу
}

// Метод для ротации лог-файлов
void Logger::rotateLogs() {
    if (logFile.is_open()) {
        logFile.close(); // Закрываем текущий файл
    }

    // Переименовываем текущий файл
    std::filesystem::path oldLogPath = logFilePath;
    oldLogPath.replace_filename("debug_old.log");

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
void Logger::init(const std::string& filename) {
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
void Logger::log(const std::string& message, const std::string& level) {
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