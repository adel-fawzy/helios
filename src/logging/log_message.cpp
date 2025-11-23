#include "log_message.hpp"

#include <iomanip>

namespace {

/**
 * @brief Converts the log level to a relevant string.
 */
const char *levelToString(helios::logging::LogLevel lvl) {
  switch (lvl) {
  case helios::logging::LogLevel::Debug:
    return "DEBUG";
  case helios::logging::LogLevel::Info:
    return "INFO";
  case helios::logging::LogLevel::Warning:
    return "WARN";
  case helios::logging::LogLevel::Error:
    return "ERROR";
  }
  return "UNKNOWN";
}

} // namespace

namespace helios::logging {

LogMessage::LogMessage(LogLevel level, std::string tag, Callback cb)
    : level_(level), tag_(std::move(tag)), callback_(std::move(cb)),
      timestamp_(std::chrono::system_clock::now()) {}

LogMessage::~LogMessage() { notify(); }

void LogMessage::notify() {
  std::ostringstream final;
  auto t = std::chrono::system_clock::to_time_t(timestamp_);
  auto tm = *std::localtime(&t);
  // Format time
  final << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "]";
  // Format log level
  std::string logLevel{levelToString(level_)};
  switch (level_) {
  case LogLevel::Info:
  case LogLevel::Warning:
    final << " [" << levelToString(level_) << "] ";
    break;
  default:
    final << " [" << levelToString(level_) << "]";
  }
  // Format tag
  final << " [" << tag_ << "] ";

  final << buffer_.str();
  callback_(final.str());
}

} // namespace helios::logging
