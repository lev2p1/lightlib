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
    static std::ofstream logFile; // ����� ��� ������ � ���-����
    static std::mutex logMutex; // ������� ��� ������������������
    static std::filesystem::path logFilePath; // ���� � ���-�����
    static size_t maxLines; // ������������ ���������� ����� (10 000)
    static size_t currentLineCount; // ������� ���������� �����

    // RAII-������ ��� ������ �������
    struct FlushOnExit {
        ~FlushOnExit() {
            if (logFile.is_open()) {
                logFile.flush();
            }
        }
    };

    static FlushOnExit flushOnExit;

    // ����������� ����� ��� ��������� ��������
    static void signalHandler(int signal);

    // ����� ��� ������� ���-������
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

    // ������������� �������
    static void init(const std::string& filename);

    // ����������� ���������
    static void log(const std::string& message, const std::string& level = "INFO");

    // ����� ��� ����������� ������������ ��������
    static void registerSignalHandlers() {
        std::signal(SIGINT, signalHandler);  // Ctrl+C
        std::signal(SIGTERM, signalHandler); // ������� kill
    }
};


// ����������� ����������� ������
inline std::ofstream Logger::logFile;
inline std::mutex Logger::logMutex;
inline std::filesystem::path Logger::logFilePath;
inline size_t Logger::maxLines = 10000; // ������������ ���������� �����
inline size_t Logger::currentLineCount = 0; // ������� ���������� �����
inline Logger::FlushOnExit Logger::flushOnExit;

// ����� ��� ��������� ��������
inline void Logger::signalHandler(int signal) {
    log("Program terminated with signal: " + std::to_string(signal), "ERROR");
    log("Application finished", "INFO");
    std::exit(signal); // ��������� ���������
}

// ����� ��� ������� ���-������
inline void Logger::rotateLogs() {
    if (logFile.is_open()) {
        logFile.close(); // ��������� ������� ����
    }

    // ��������������� ������� ����
    std::filesystem::path oldLogPath = logFilePath;

    std::string result = oldLogPath.filename().string(); // �������� �������� ������
    size_t pos = result.find(oldLogPath.extension().string()); // ���� ���������

    if (pos != std::string::npos) { // ���� ��������� �������
        result.erase(pos, oldLogPath.extension().string().length()); // ������� ���������
    }

    oldLogPath.replace_filename(result + "_old" + oldLogPath.extension().string());

    // ������� ������ ����, ���� �� ����������
    if (std::filesystem::exists(oldLogPath)) {
        std::filesystem::remove(oldLogPath);
    }

    // ��������������� ������� ���� � ������
    std::filesystem::rename(logFilePath, oldLogPath);

    // ������� ����� ����
    logFile.open(logFilePath, std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to open log file: " + logFilePath.string());
    }

    currentLineCount = 0; // ���������� ������� �����
}

// ������������� �������
inline void Logger::init(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex);

    // �������� ���� � ����� Storage/Logs
    std::filesystem::path logDir = std::filesystem::current_path() / "Storage" / "Logs";

    // ������� �����, ���� ��� �� ����������
    if (!std::filesystem::exists(logDir)) {
        std::filesystem::create_directories(logDir);
    }

    // ��������� ������ ���� � ���-�����
    logFilePath = logDir / filename;

    // ��������� ���� ��� ������
    logFile.open(logFilePath, std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to open log file: " + logFilePath.string());
    }

    // �������������� ������� �����
    std::ifstream inFile(logFilePath);
    currentLineCount = std::count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n');
    inFile.close();
}

// ����������� ���������
inline void Logger::log(const std::string& message, const std::string& level) {
    std::lock_guard<std::mutex> lock(logMutex);
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);

    if (logFile.is_open()) {
        logFile << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " [" << level << "] " << message << std::endl;
        logFile.flush(); // ���������� ����� �� ����

        // ����������� ������� �����
        currentLineCount++;

        // ���������, ����� �� ��������� �������
        if (currentLineCount >= maxLines) {
            rotateLogs();
        }
    }
    std::cout << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " [" << level << "] " << message << std::endl;
}