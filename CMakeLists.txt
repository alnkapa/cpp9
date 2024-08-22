cmake_minimum_required(VERSION 3.25.1)

set(CMAKE_CXX_STANDARD 20)  # Установите стандарт C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Обязательно использовать указанный стандарт
set(CMAKE_CXX_EXTENSIONS OFF)  # Отключите расширения компилятора (например, GNU)

project(Bayan VERSION 0.0.1)

add_executable(bayan file_finder.cpp main.cpp)

target_include_directories(
    bayan 
    PRIVATE
    ${CMAKE_SOURCE_DIR}
)

add_subdirectory(argc)
add_subdirectory(global)
add_subdirectory(hashing)
add_subdirectory(files)

target_link_libraries(
    bayan
    PRIVATE
    global
    argc
    hashing
    files    
)

set(CMAKE_BUILD_TYPE Debug)
