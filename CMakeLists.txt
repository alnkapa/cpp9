# my_project/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Указываем, что мы будем использовать CMake для подкаталога library
add_subdirectory(library)

# Создание исполняемого файла
add_executable(my_project src/main.cpp)

# Подключение библиотеки к исполняемому файлу
target_link_libraries(my_project PRIVATE async)
