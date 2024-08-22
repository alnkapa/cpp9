cmake_minimum_required(VERSION 3.25.1)


add_library(
    hashing 
    INTERFACE
)

target_include_directories(
    hashing 
    INTERFACE
    ${CMAKE_SOURCE_DIR}
)

find_package(OpenSSL REQUIRED)

target_link_libraries(
    hashing
    INTERFACE
    OpenSSL::SSL 
    OpenSSL::Crypto
)
