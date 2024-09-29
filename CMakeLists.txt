cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(async)
# find_library(ASYNC_LIB NAMES async PATHS /usr)

add_executable(bulk src/main.cpp src/status.cpp)



target_link_libraries(bulk PRIVATE async)
