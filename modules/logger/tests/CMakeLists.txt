include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)

add_executable(logger_tests
    logger_test.cpp
)

target_link_libraries(logger_tests
    PRIVATE
        logger
        GTest::gtest
        GTest::gtest_main
)

# Register with CTest
include(GoogleTest)
gtest_discover_tests(logger_tests)
