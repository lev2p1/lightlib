# Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(Light)

# Указываем путь к vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/Tools/vcpkg-master/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

set(CMAKE_CXX_STANDART 20)

# Находим и подключаем зависимости
find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(jwt_cpp REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# Добавляем зависимости к вашему проекту
target_link_libraries(Light
    PRIVATE
    Boost::boost
    fmt::fmt
    jwt_cpp::jwt_cpp
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    ZLIB::ZLIB
)