#pragma once

#include "log_config.hpp"
#include "log_level.hpp"

namespace helios::logger {

constexpr bool LOG_ENABLED(LogLevel lvl) {
  return static_cast<int>(lvl) >= static_cast<int>(LOG_LEVEL_MIN);
}

} // namespace helios::logger

#define LOG_DEBUG                                                              \
  if (helios::logger::LOG_ENABLED(helios::logger::LogLevel::Debug))            \
  logger_.debug()

#define LOG_INFO                                                               \
  if (helios::logger::LOG_ENABLED(helios::logger::LogLevel::Info))             \
  logger_.info()

#define LOG_WARN                                                               \
  if (helios::logger::LOG_ENABLED(helios::logger::LogLevel::Warning))          \
  logger_.warn()

#define LOG_ERROR                                                              \
  if (helios::logger::LOG_ENABLED(helios::logger::LogLevel::Error))            \
  logger_.error()
