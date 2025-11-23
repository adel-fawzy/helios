#include "logger.hpp"

namespace helios::logging {

Logger::Logger(std::string name,
               std::vector<std::shared_ptr<ISink>> sinks)
    : name_{name}, sinks_{sinks} {}

LogMessage Logger::debug() { return make(LogLevel::Debug); }
LogMessage Logger::info() { return make(LogLevel::Info); }
LogMessage Logger::warn() { return make(LogLevel::Warning); }
LogMessage Logger::error() { return make(LogLevel::Error); }

LogMessage Logger::make(LogLevel lvl) {
  return LogMessage(lvl, name_, [sinksCopy = sinks_](const std::string &line) {
    for (auto &sink : sinksCopy)
      sink->write(line);
  });
}

} // namespace helios::logging
