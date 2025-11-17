cmake_minimum_required(VERSION 3.16)

project(Helios VERSION 1.0.0 LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# 1. Create the shared library
#
add_library(helios SHARED
    src/core/event_queue/event_queue.cpp
    src/core/signal_bus/signal_bus.cpp
    src/core/event_loop/event_loop.cpp
)

# Link with the standard threads library
find_package(Threads REQUIRED)
target_link_libraries(helios PRIVATE Threads::Threads)

# Tell consumers where headers are
target_include_directories(helios PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/helios>
)

target_include_directories(helios PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

#
# 2. Install headers and the library
#
install(DIRECTORY include/
    DESTINATION include/helios
)

install(TARGETS helios
    EXPORT HeliosTargets
    LIBRARY DESTINATION lib
)

#
# 3. Install CMake package files so find_package(Helios) works
#
include(CMakePackageConfigHelpers)

set(CONFIG_INSTALL_DIR lib/cmake/Helios)

# Generate version file (HeliosConfigVersion.cmake)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/HeliosConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Generate main config file (HeliosConfig.cmake)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/HeliosConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/HeliosConfig.cmake"
    INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
)

# Install both config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/HeliosConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/HeliosConfigVersion.cmake"
    DESTINATION ${CONFIG_INSTALL_DIR}
)

# Install the exported target file (HeliosTargets.cmake)
install(EXPORT HeliosTargets
    FILE HeliosTargets.cmake
    NAMESPACE Helios::
    DESTINATION ${CONFIG_INSTALL_DIR}
)

option(BUILD_TESTS "Build unit tests" ON)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
