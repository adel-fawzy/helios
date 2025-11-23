#pragma once

#include "log_config.hpp"
#include "log_level.hpp"

namespace helios::logger {

constexpr bool LOG_ENABLED(LogLevel lvl) {
    return static_cast<int>(lvl) >= static_cast<int>(LOG_LEVEL_MIN);
}

} // namespace helios::logger

#define LOG_DEBUG(logger) \
    if (helios::logger::LOG_ENABLED(helios::logger::LogLevel::Debug)) \
        logger->debug()

#define LOG_INFO(logger) \
    if (helios::logger::LOG_ENABLED(helios::logger::LogLevel::Info)) \
        logger->info()

#define LOG_WARN(logger) \
    if (helios::logger::LOG_ENABLED(helios::logger::LogLevel::Warning)) \
        logger->warn()

#define LOG_ERROR(logger) \
    if (helios::logger::LOG_ENABLED(helios::logger::LogLevel::Error)) \
        logger->error()
