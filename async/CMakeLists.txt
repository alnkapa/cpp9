# library/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(async_library)

# Создание библиотеки
add_library(async STATIC async.cpp)

# Указание директории с заголовочными файлами
target_include_directories(async PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
