add_library(timesys SHARED)

target_sources(timesys
    PRIVATE
        src/timers_manager.cpp
)

target_include_directories(timesys
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Private dependencies
find_package(Threads REQUIRED)
target_link_libraries(timesys
    PRIVATE
        core
        Threads::Threads
)

set_target_properties(timesys PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Installation
install(TARGETS timesys
        EXPORT HeliosTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include/ DESTINATION include)

enable_testing()
add_subdirectory(tests)
