#Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

#Название проекта
project(Light)

#Указываем путь к vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/Tools/vcpkg-master/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

set(CMAKE_CXX_STANDART 20)
#Ищем hiredis
find_package(hiredis REQUIRED PATHS "C:/Tools/vcpkg-master/installed/x64-windows/share/hiredis")

#Добавляем исполняемый файл
add_executable(Light Main.cpp)

#Подключаем hiredis к проекту
target_link_libraries(Light PRIVATE hiredis::hiredis)