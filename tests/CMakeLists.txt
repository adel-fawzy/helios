# ---- Fetch GoogleTest ----
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)

enable_testing()

# ---- Test executable ----
add_executable(helios_tests
    EventQueue/TestDefaultEventQueue.cpp
)

target_include_directories(helios_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(helios_tests PRIVATE
    helios
    GTest::gtest
    GTest::gtest_main
)

# ---- Auto-discover tests ----
include(GoogleTest)
gtest_discover_tests(helios_tests)
