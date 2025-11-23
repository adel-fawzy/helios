#pragma once

#include "logging/log_config.hpp"
#include "logging/log_level.hpp"

namespace helios::logging {

constexpr bool LOG_ENABLED(LogLevel lvl) {
    return static_cast<int>(lvl) >= static_cast<int>(LOG_LEVEL_MIN);
}

} // namespace helios::logging

#define LOG_DEBUG(logger) \
    if (helios::logging::LOG_ENABLED(helios::logging::LogLevel::Debug)) \
        logger->debug()

#define LOG_INFO(logger) \
    if (helios::logging::LOG_ENABLED(helios::logging::LogLevel::Info)) \
        logger->info()

#define LOG_WARN(logger) \
    if (helios::logging::LOG_ENABLED(helios::logging::LogLevel::Warning)) \
        logger->warn()

#define LOG_ERROR(logger) \
    if (helios::logging::LOG_ENABLED(helios::logging::LogLevel::Error)) \
        logger->error()
