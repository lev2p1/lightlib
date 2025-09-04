cmake_minimum_required(VERSION 3.14)
project(light VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

if(WIN32)
    set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows" CACHE STRING "")
elseif(UNIX)
    set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux" CACHE STRING "")
endif()

if(MSVC)
    add_compile_options(/utf-8)
endif()

find_package(Boost 1.86 CONFIG REQUIRED COMPONENTS beast asio filesystem system thread date_time smart_ptr optional variant algorithm)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(hiredis CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PostgreSQL REQUIRED)


add_executable(light Main.cpp)

target_include_directories(light PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(WIN32)
    target_include_directories(light PRIVATE ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/include)
endif()

target_link_libraries(light
    PRIVATE
    Boost::beast  
    Boost::asio
    Boost::filesystem
    Boost::system
    Boost::thread
    Boost::date_time
    Boost::smart_ptr
    Boost::optional
    Boost::variant
    Boost::algorithm
    fmt::fmt
    nlohmann_json::nlohmann_json
    jwt-cpp::jwt-cpp
    hiredis::hiredis
    ZLIB::ZLIB
    PostgreSQL::PostgreSQL
)
