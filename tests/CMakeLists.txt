include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)

add_executable(helios_tests
    core/event_queue.cpp
    core/signal_bus.cpp
    core/event_loop.cpp
    # core/module.cpp
    # logging/standard_output_sink.cpp
    # logging/file_sink.cpp
    logging/logger.cpp
)

target_include_directories(helios_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(helios_tests PRIVATE
    helios
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(helios_tests)
