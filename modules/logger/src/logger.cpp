#include "logger/logger.hpp"

#include <core/event_loop.hpp>

#include "log_message.hpp"
#include "file_sink.hpp"
#include "log_config.hpp"
#include "standard_output_sink.hpp"

namespace helios::logger {

class Logger::Impl {
public:
  Impl(std::string name);
  ~Impl() = default;

  /**
   * @brief Creates and returns a 'LogMessageFactory'.
   *
   * @return 'LogMessageFactory'.
   */
  LogMessageFactory make(LogLevel lvl);

private:
  /**
   * @brief Name of the logger.
   */
  const std::string name_;

  /**
   * @brief Runs the sinks.
   */
  static core::EventLoop loop_;

  /**
   * @brief Signal bus that holds the signals for logging.
   */
  static std::shared_ptr<core::SignalBus> logBus_;

  /**
   * @brief Unique pointer to the standard output sink.
   */
  static std::shared_ptr<StandardOutputSink> stdOutSink_;

  /**
   * @brief Unique pointer to the file sink.
   */
  static std::shared_ptr<FileSink> fileSink_;

  /**
   * @brief Indicates if the log sinks are created or not yet.
   */
  static std::once_flag isSinksInitialized_;

  /**
   * @brief Checks the configured log sinks and creates them.
   */
  static void initSinks();
}; // class Impl

Logger::Logger(std::string name)
    : impl_{std::make_unique<Logger::Impl>(std::move(name))} {}

Logger::~Logger() = default;

LogMessageFactory Logger::debug() { return impl_->make(LogLevel::Debug); }
LogMessageFactory Logger::info() { return impl_->make(LogLevel::Info); }
LogMessageFactory Logger::warn() { return impl_->make(LogLevel::Warning); }
LogMessageFactory Logger::error() { return impl_->make(LogLevel::Error); }

Logger::Impl::Impl(std::string name) : name_{name} {}

std::shared_ptr<core::SignalBus> Logger::Impl::logBus_{
    std::make_shared<core::SignalBus>()};

core::EventLoop Logger::Impl::loop_{};

std::shared_ptr<StandardOutputSink> Logger::Impl::stdOutSink_;

std::shared_ptr<FileSink> Logger::Impl::fileSink_;

std::once_flag Logger::Impl::isSinksInitialized_;

LogMessageFactory Logger::Impl::make(LogLevel lvl) {
  std::call_once(isSinksInitialized_, [] { Logger::Impl::initSinks(); });
  return LogMessageFactory(lvl, name_, [this](std::string msg) {
    logBus_->publish<LogMessage>(std::move(msg));
  });
}

void Logger::Impl::initSinks() {
  if constexpr (ENABLE_STDOUT_SINK) {
    stdOutSink_ = std::make_unique<StandardOutputSink>(logBus_);
  }

  if constexpr (ENABLE_FILE_SINK) {
    fileSink_ = std::make_unique<FileSink>(logBus_, LOG_FILE_PATH);
  }

  if (stdOutSink_) {
    loop_.add(stdOutSink_);
  }

  if (fileSink_) {
    loop_.add(fileSink_);
  }

  if (stdOutSink_ || fileSink_)
    loop_.run();
}

} // namespace helios::logger
