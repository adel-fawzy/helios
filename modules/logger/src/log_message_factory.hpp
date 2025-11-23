#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <sstream>
#include <string>

#include "core/signal_bus.hpp"
#include "log_level.hpp"

namespace helios::logger {

/**
 * @class logger::LogMessage
 *
 * @brief A log message that contains relevant information.
 *
 * @details
 * - The log message contains the following:
 *    1. Time stamp
 *    2. Tag
 *    3. Log level
 *    4. Log message
 * - All public functions are not thread-safe.
 * - All public functions are synchronous.
 */
class LogMessageFactory {
public:
  /**
   * @brief Constructor.
   *
   * @param level Log level.
   * @param tag Tag name.
   * @param signalBus Signal bus to publish final log message.
   */
  LogMessageFactory(LogLevel level, std::string tag,
                    std::shared_ptr<core::SignalBus> signalBus);

  /**
   * @brief Destructor.
   */
  ~LogMessageFactory();

  /**
   * @brief Overloaded operator<<.
   *
   * @tparam MessageType Type of message to be logged.
   */
  template <typename MessageType>
  LogMessageFactory &operator<<(const MessageType &value) {
    buffer_ << value;
    return *this;
  }

private:
  /**
   * @brief Log level.
   */
  LogLevel level_;

  /**
   * @brief Tag.
   */
  std::string tag_;

  /**
   * @brief Used for publishing the final log message.
   */
  std::shared_ptr<core::SignalBus> signalBus_;

  /**
   * @brief Time stamp when the object was created.
   */
  std::chrono::system_clock::time_point timestamp_;

  /**
   * @brief Buffer to hold the log message in between calls.
   */
  std::ostringstream buffer_;

  /**
   * @brief Publishes the final log message.
   */
  void publish();

  /**
   * @brief Formats 'buffer_'.
   *
   * @return The formatted string.
   */
  std::string format() const;
}; // class LogMessage

} // namespace helios::logger
