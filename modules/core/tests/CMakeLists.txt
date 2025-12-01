include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)

add_executable(core_tests
    # hobject_test.cpp
    long_running_thread_test.cpp
    event_loop_test.cpp
    future_result_test.cpp
)

target_include_directories(core_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(core_tests
    PRIVATE
        core
        GTest::gtest
        GTest::gtest_main
)

# Register with CTest
include(GoogleTest)
gtest_discover_tests(core_tests)
