#include "logger/logger.hpp"

#include <core/h_loop.hpp>
#include <vector>

#include "file_sink.hpp"
#include "log_config.hpp"
#include "log_message.hpp"
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
  static std::shared_ptr<core::HLoop> loop_;

  /**
   * @brief Signal bus that holds the signals for logging.
   */
  static core::HBus logBus_;

  /**
   * @brief Vector of shared pointers to sinks.
   */
  static std::vector<std::shared_ptr<core::HObject>> sinks_;

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

std::shared_ptr<core::HLoop> Logger::Impl::loop_{
    std::make_shared<core::HLoop>()
};

core::HBus Logger::Impl::logBus_{};

std::vector<std::shared_ptr<core::HObject>> Logger::Impl::sinks_;

std::once_flag Logger::Impl::isSinksInitialized_;

LogMessageFactory Logger::Impl::make(LogLevel lvl) {
  std::call_once(isSinksInitialized_, [] { Logger::Impl::initSinks(); });
  return LogMessageFactory(lvl, name_, [this](std::string msg) {
    logBus_.publish<LogMessage>(std::move(msg));
  });
}

void Logger::Impl::initSinks() {
  if constexpr (ENABLE_STDOUT_SINK) {
    sinks_.emplace_back(std::make_shared<StandardOutputSink>(loop_, &logBus_));
  }

  if constexpr (ENABLE_FILE_SINK) {
    sinks_.emplace_back(std::make_shared<FileSink>(loop_, &logBus_));
  }
}

} // namespace helios::logger
