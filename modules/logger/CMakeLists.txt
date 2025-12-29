include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake)

add_library(logger SHARED)

target_sources(logger
    PRIVATE
        src/file_sink.cpp
        src/log_message_factory.cpp
        src/logger.cpp
        src/standard_output_sink.cpp
)

target_include_directories(logger
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/gen/logger>
        $<INSTALL_INTERFACE:include>
)

# Private dependencies
target_link_libraries(logger
    PRIVATE
        core
)

set_target_properties(logger PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Installation
install(TARGETS logger
        EXPORT HeliosTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gen/logger DESTINATION include)

enable_testing()
add_subdirectory(tests)
