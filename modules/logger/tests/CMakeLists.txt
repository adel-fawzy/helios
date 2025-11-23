include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)

add_executable(test_logger test_logger.cpp)

target_link_libraries(test_logger PRIVATE logger GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_logger)
