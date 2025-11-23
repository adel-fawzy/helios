include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake)

add_library(logger OBJECT)

set_target_properties(core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(logger PRIVATE
    src/hello.cpp
    # src/file_sink.cpp
    # src/log_message_factory.cpp
    # src/logger.cpp
    # src/standard_output_sink.cpp
)

target_include_directories(logger PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_include_directories(logger PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gen/logger)

target_link_libraries(logger PUBLIC core)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
