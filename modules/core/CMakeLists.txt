add_library(core OBJECT)

set_target_properties(core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(core PRIVATE
    src/event_loop.cpp
    src/event_queue.cpp
    src/signal_bus.cpp
    src/h_object.cpp
)

# Private dependencies
find_package(Threads REQUIRED)
target_link_libraries(core PRIVATE Threads::Threads)

target_include_directories(core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_subdirectory(tests)
