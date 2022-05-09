cmake_minimum_required(VERSION 3.5)
project(aamptoyaml)

add_subdirectory(lib/oead)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(aamptoyaml src/main.cpp)
target_link_libraries(aamptoyaml PUBLIC oead)