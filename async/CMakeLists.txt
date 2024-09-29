# library/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(async_library)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Создание библиотеки
add_library(async SHARED async.cpp)

# Указание директории с заголовочными файлами
target_include_directories(async PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(FILES async.h DESTINATION include)
install(TARGETS async DESTINATION lib)


set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "async")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION "asynchronous library")
set(CPACK_PACKAGE_CONTACT "my@my.my") 

include(CPack)