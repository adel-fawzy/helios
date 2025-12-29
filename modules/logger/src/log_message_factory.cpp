#include "logger/log_message_factory.hpp"

#include "log_message.hpp"
#include <iomanip>

namespace {

/**
 * @brief Converts the log level to a relevant string.
 */
const char *levelToString(helios::logger::LogLevel lvl) {
  switch (lvl) {
  case helios::logger::LogLevel::Debug:
    return "DEBUG";
  case helios::logger::LogLevel::Info:
    return "INFO";
  case helios::logger::LogLevel::Warning:
    return "WARN";
  case helios::logger::LogLevel::Error:
    return "ERROR";
  }
  return "UNKNOWN";
}

} // namespace

namespace helios::logger {

LogMessageFactory::LogMessageFactory(LogLevel level, std::string tag,
                                     std::function<void(std::string)> cb)
    : level_(level), tag_(std::move(tag)), cb_(cb),
      timestamp_(std::chrono::system_clock::now()) {}

LogMessageFactory::~LogMessageFactory() { cb_(format()); }

std::string LogMessageFactory::format() const {
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

  // Append actual log msg
  final << buffer_.str();

  return final.str();
}

} // namespace helios::logger
