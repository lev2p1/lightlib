# Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(Light)

# Указываем путь к vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/Tools/vcpkg-master/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

set(CMAKE_CXX_STANDART 20)

# Добавляем исполняемый файл
add_executable(Light Main.cpp)
