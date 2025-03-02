#pragma once

#include <windows.h>
#include <bcrypt.h>
#include <iostream>
#include <vector>
#include <string>


#pragma comment(lib, "bcrypt.lib")

class Hash {
public:

    static std::vector<BYTE> self_salt;

    // Генерация соли
    static std::vector<BYTE> generateSalt(size_t length) {
        std::vector<BYTE> salt(length);

        // Используем криптографически безопасный генератор случайных чисел
        NTSTATUS status = BCryptGenRandom(
            nullptr,              // Используем системный провайдер
            salt.data(),          // Указатель на буфер для соли
            static_cast<ULONG>(salt.size()), // Размер соли
            BCRYPT_USE_SYSTEM_PREFERRED_RNG // Флаги
        );

        if (status != 0) {
            throw std::runtime_error("Failed to generate salt. Error code: " + std::to_string(status));
        }

        return salt;
    }

    // Хэширование пароля с солью и итерациями
    static std::string hash(const std::string& password, ULONG iterations) {
        constexpr DWORD hashLength = 64; // Длина хэша (в байтах)
        std::vector<BYTE> hash(hashLength);

        // Открываем алгоритм SHA-512
        BCRYPT_ALG_HANDLE hAlgorithm = nullptr;
        NTSTATUS status = BCryptOpenAlgorithmProvider(
            &hAlgorithm,          // Указатель на хэндл алгоритма
            BCRYPT_SHA512_ALGORITHM, // Используем SHA-512
            nullptr,              // Провайдер (nullptr для системного)
            0                     // Флаги
        );

        if (status != 0) {
            throw std::runtime_error("Failed to open algorithm provider. Error code: " + std::to_string(status));
        }

        // Создаем хэш-объект
        BCRYPT_HASH_HANDLE hHash = nullptr;
        status = BCryptCreateHash(
            hAlgorithm,           // Хэндл алгоритма
            &hHash,               // Указатель на хэндл хэш-объекта
            nullptr,              // Буфер для хэш-объекта (не требуется)
            0,                    // Размер буфера (не требуется)
            nullptr,              // Соль (не передаём соль здесь)
            0,                    // Длина соли (0, так как соль не передаётся)
            0                     // Флаги
        );

        if (status != 0) {
            BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            throw std::runtime_error("Failed to create hash object. Error code: " + std::to_string(status));
        }

        // Добавляем соль в хэш-объект
        status = BCryptHashData(
            hHash,                // Хэндл хэш-объекта
            const_cast<PUCHAR>(self_salt.data()), // Соль
            static_cast<ULONG>(self_salt.size()), // Длина соли
            0                     // Флаги
        );

        if (status != 0) {
            BCryptDestroyHash(hHash);
            BCryptCloseAlgorithmProvider(hAlgorithm, 0);
            throw std::runtime_error("Failed to add salt to hash. Error code: " + std::to_string(status));
        }

        // Хэшируем пароль с итерациями
        for (ULONG i = 0; i < iterations; ++i) {
            status = BCryptHashData(
                hHash,            // Хэндл хэш-объекта
                reinterpret_cast<PUCHAR>(const_cast<char*>(password.data())), // Пароль
                static_cast<ULONG>(password.size()), // Длина пароля
                0                  // Флаги
            );

            if (status != 0) {
                BCryptDestroyHash(hHash);
                BCryptCloseAlgorithmProvider(hAlgorithm, 0);
                throw std::runtime_error("Failed to hash data. Error code: " + std::to_string(status));
            }
        }

        // Завершаем хэширование и получаем хэш
        status = BCryptFinishHash(
            hHash,               // Хэндл хэш-объекта
            hash.data(),          // Выходной буфер для хэша
            hashLength,           // Длина хэша
            0                     // Флаги
        );

        // Закрываем хэш-объект и алгоритм
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlgorithm, 0);

        if (status != 0) {
            throw std::runtime_error("Failed to finish hash. Error code: " + std::to_string(status));
        }

        // Преобразуем хэш в строку
        std::string result;
        for (BYTE b : hash) {
            char buf[3];
            snprintf(buf, sizeof(buf), "%02x", b);
            result += buf;
        }

        return result;
    }

    static inline std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
        std::vector<uint8_t> bytes;
        for (size_t i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(strtol(byteString.c_str(), nullptr, 16));
            bytes.push_back(byte);
        }
        return bytes;
    }
};