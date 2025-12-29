# Log file path
set(LOG_FILE_PATH "logs/app.log" CACHE STRING "Path of the log file")

# Logging sink options
option(LOG_SINK_STDOUT "Enable stdout logging sink" ON)
option(LOG_SINK_FILE "Enable file logging sink" OFF)

# Minimum logging level
set(MIN_LOG_LEVEL "Debug" CACHE STRING "Minimum compile-time log level")
set_property(CACHE MIN_LOG_LEVEL PROPERTY STRINGS Debug Info Warning Error)

# Generate log_config.hpp
file(MAKE_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/gen/logging
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/gen/logger/log_config.hpp
)
