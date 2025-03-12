#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <map>
#include <fstream>
#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

class Storage {
private:
    // Приватный конструктор для Singleton
    Storage() {}

    // Запрет копирования и присваивания
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    // Корневая директория для хранения файлов
    std::string rootPath;

public:
    // Метод для получения единственного экземпляра класса
    static Storage& getInstance() {
        static Storage instance;
        return instance;
    }

    // Установка корневой директории
    void setRootPath(const std::string& path) {
        rootPath = path;
        if (!fs::exists(rootPath)) {
            fs::create_directories(rootPath);
        }
    }

    // Получение корневой директории
    std::string getRootPath() const {
        return rootPath;
    }

    // Сохранение файла
    void put(const std::string& path, const std::string& content) {
        std::ofstream file(rootPath + "/" + path, std::ios::out);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + path);
        }
        file << content;
        file.close();
    }

    // Чтение файла
    std::string get(const std::string& path) {
        std::ifstream file(rootPath + "/" + path, std::ios::in);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for reading: " + path);
        }
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return content;
    }

    // Проверка существования файла
    bool exists(const std::string& path) {
        return fs::exists(rootPath + "/" + path);
    }

    // Удаление файла
    void deleteFile(const std::string& path) {
        if (!exists(path)) {
            throw std::runtime_error("File does not exist: " + path);
        }
        fs::remove(rootPath + "/" + path);
    }

    // Копирование файла
    void copy(const std::string& source, const std::string& destination) {
        if (!exists(source)) {
            throw std::runtime_error("Source file does not exist: " + source);
        }
        fs::copy(rootPath + "/" + source, rootPath + "/" + destination);
    }

    // Перемещение файла
    void move(const std::string& source, const std::string& destination) {
        if (!exists(source)) {
            throw std::runtime_error("Source file does not exist: " + source);
        }
        fs::rename(rootPath + "/" + source, rootPath + "/" + destination);
    }
};

#endif