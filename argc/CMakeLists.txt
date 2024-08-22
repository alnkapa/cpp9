cmake_minimum_required(VERSION 3.25.1)


add_library(
    argc 
    STATIC    
    argc.cpp 
    argc.h
)

target_include_directories(
    argc 
    PUBLIC
    ${CMAKE_SOURCE_DIR}
)

find_package(Boost REQUIRED COMPONENTS program_options)

target_link_libraries(
    argc
    PRIVATE
    Boost::program_options
)

