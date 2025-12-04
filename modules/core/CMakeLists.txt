add_library(core SHARED)

target_sources(core
    PRIVATE
        # src/event_loop.cpp
        # src/event_queue.cpp
        src/signal_bus.cpp
        src/in_active_h_object.cpp
        # src/long_running_thread.cpp
        src/active_h_object.cpp
)

target_include_directories(core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Private dependencies
find_package(Threads REQUIRED)
target_link_libraries(core
    PRIVATE
        Threads::Threads
)

set_target_properties(core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Installation
install(TARGETS core
        EXPORT HeliosTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include/ DESTINATION include)

enable_testing()
add_subdirectory(tests)
