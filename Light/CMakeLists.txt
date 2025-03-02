cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)

# Указываем путь к Boost
set(BOOST_ROOT "D:/boost/boost_1_87_0/boost_1_87_0")
set(BOOST_LIBRARYDIR "D:/boost/boost_1_87_0/boost_1_87_0/stage/lib")

# Ищем Boost
find_package(Boost REQUIRED COMPONENTS system filesystem unit_test_framework)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

# Определение _WIN32_WINNT
add_definitions(-D_WIN32_WINNT=0x0601)

# Основное приложение
add_executable(MyApp Main.cpp)
target_include_directories(MyApp PRIVATE .)
target_link_libraries(MyApp PRIVATE Boost::system Boost::filesystem)

# Тесты
add_executable(Tests Tests/Test.cpp)
target_include_directories(Tests PRIVATE .)
target_link_libraries(Tests PRIVATE Boost::system Boost::filesystem Boost::unit_test_framework)
