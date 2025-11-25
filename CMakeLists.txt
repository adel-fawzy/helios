cmake_minimum_required(VERSION 3.16)

project(Helios VERSION 1.0.0 LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

# -----------------------------------------
# Create final shared library
# -----------------------------------------
add_library(helios SHARED)

# -----------------------------------------
# Add modules
# -----------------------------------------
add_subdirectory(modules/core)
add_subdirectory(modules/logger)

# Add object files from modules
target_sources(helios PRIVATE
    $<TARGET_OBJECTS:core>
    $<TARGET_OBJECTS:logger>
)

# -----------------------------------------
# Includes
# -----------------------------------------
target_include_directories(helios
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/modules>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/gen>
        $<INSTALL_INTERFACE:include/helios>
)

# -----------------------------------------
# Install headers
# Each module installs its own headers inside include/helios/<module>/
# -----------------------------------------
install(DIRECTORY modules/
    DESTINATION include/helios
    FILES_MATCHING
        PATTERN "include/*"
)

# -----------------------------------------
# Install the shared library
# -----------------------------------------
install(TARGETS helios
    EXPORT HeliosTargets
    LIBRARY DESTINATION lib
)

# -----------------------------------------
# CMake package configuration
# -----------------------------------------
include(CMakePackageConfigHelpers)

set(CONFIG_INSTALL_DIR lib/cmake/Helios)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/HeliosConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/HeliosConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/HeliosConfig.cmake"
    INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/HeliosConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/HeliosConfigVersion.cmake"
    DESTINATION ${CONFIG_INSTALL_DIR}
)

install(EXPORT HeliosTargets
    FILE HeliosTargets.cmake
    NAMESPACE Helios::
    DESTINATION ${CONFIG_INSTALL_DIR}
)
