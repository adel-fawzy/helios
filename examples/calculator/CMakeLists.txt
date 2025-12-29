cmake_minimum_required(VERSION 3.16)
project(Calculator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Helios REQUIRED)

add_executable(calculator src/main.cpp)

target_link_libraries(calculator PRIVATE Helios::core)
