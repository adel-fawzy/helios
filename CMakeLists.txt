cmake_minimum_required(VERSION 3.16)

project(Helios VERSION 1.0.0 LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add modules
add_subdirectory(modules/core)
add_subdirectory(modules/logger)
add_subdirectory(modules/timesys)
