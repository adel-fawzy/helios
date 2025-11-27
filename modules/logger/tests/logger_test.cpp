#include "logger/logger.hpp"

#include <gtest/gtest.h>

#include "logger/log_macros.hpp"

TEST(LoggerTest, DebugNoMacros) {
  helios::logger::Logger logger_("LoggerTest");
  logger_.debug() << "I am a debug log";
}

TEST(LoggerTest, InfoNoMacros) {
  helios::logger::Logger logger_("LoggerTest");
  logger_.info() << "I am an info log";
}

TEST(LoggerTest, WarnNoMacros) {
  helios::logger::Logger logger_("LoggerTest");
  logger_.warn() << "I am a warning log";
}

TEST(LoggerTest, ErrNoMacros) {
  helios::logger::Logger logger_("LoggerTest");
  logger_.error() << "I am an error log";
}

TEST(LoggerTest, DebugMacros) {
  helios::logger::Logger logger_("LoggerTest");
  LOG_DEBUG << "I am a debug log by a macro";
}

TEST(LoggerTest, InfoMacros) {
  helios::logger::Logger logger_("LoggerTest");
  LOG_INFO << "I am an info log by a macro";
}

TEST(LoggerTest, WarnMacros) {
  helios::logger::Logger logger_("LoggerTest");
  LOG_WARN << "I am a warning log by a macro";
}

TEST(LoggerTest, ErrMacros) {
  helios::logger::Logger logger_("LoggerTest");
  LOG_ERROR << "I am an error log by a macro";
}
