#include "logger/logger.hpp"

#include "log_config.hpp"
#include "file_sink.hpp"
#include "standard_output_sink.hpp"

namespace helios::logger {

Logger::Logger(std::string name) : name_{name} {}

std::shared_ptr<core::SignalBus> Logger::signalBus_{
    std::make_shared<core::SignalBus>()};

std::shared_ptr<core::EventQueue> Logger::eventQueue_{
    std::make_shared<core::EventQueue>()};

core::EventLoop Logger::eventLoop_{eventQueue_};

std::unique_ptr<StandardOutputSink> Logger::stdOutSink_;

std::unique_ptr<FileSink> Logger::fileSink_;

std::once_flag Logger::isSinksInitialized_;

LogMessageFactory Logger::debug() { return make(LogLevel::Debug); }
LogMessageFactory Logger::info() { return make(LogLevel::Info); }
LogMessageFactory Logger::warn() { return make(LogLevel::Warning); }
LogMessageFactory Logger::error() { return make(LogLevel::Error); }

LogMessageFactory Logger::make(LogLevel lvl) {
  std::call_once(isSinksInitialized_, []() { Logger::initSinks(); });
  return LogMessageFactory(lvl, name_, signalBus_);
}

void Logger::initSinks() {
  if constexpr (ENABLE_STDOUT_SINK) {
    stdOutSink_ = std::make_unique<StandardOutputSink>(eventQueue_, signalBus_);
  }

  if constexpr (ENABLE_FILE_SINK) {
    fileSink_ =
        std::make_unique<FileSink>(eventQueue_, signalBus_, LOG_FILE_PATH);
  }

  eventLoop_.start();
}

} // namespace helios::logger
