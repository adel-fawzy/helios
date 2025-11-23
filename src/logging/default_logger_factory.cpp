#include "logging/default_logger_factory.hpp"

#include "core/event_loop.hpp"
#include "core/event_queue.hpp"
#include "file.hpp"
#include "file_sink.hpp"
#include "logger.hpp"
#include "logging/log_config.hpp"
#include "standard_output_sink.hpp"

namespace helios::logging {

DefaultLoggerFactory::DefaultLoggerFactory()
    : eventQueue_{std::make_shared<core::EventQueue>()} {
  if (ENABLE_STDOUT_SINK) {
    standardOutSink_ = std::make_shared<StandardOutputSink>(eventQueue_);
  }
  if constexpr (ENABLE_FILE_SINK) {
    auto loggingFile = std::make_unique<File>(LOG_FILE_PATH);
    fileSink_ = std::make_shared<FileSink>(std::move(loggingFile), eventQueue_);
  }
}

DefaultLoggerFactory &DefaultLoggerFactory::instance() {
  static DefaultLoggerFactory instance;
  return instance;
}

std::unique_ptr<ILogger> DefaultLoggerFactory::create(std::string name) {
  std::vector<std::shared_ptr<ISink>> sinks;

  if constexpr (ENABLE_STDOUT_SINK) {
    sinks.push_back(standardOutSink_);
  }

  if constexpr (ENABLE_FILE_SINK) {
    sinks.push_back(fileSink_);
  }

  return std::make_unique<Logger>(std::move(name), std::move(sinks));
}

std::shared_ptr<core::IEventQueue> DefaultLoggerFactory::getEventQueue() const {
  return eventQueue_;
}

} // namespace helios::logging
