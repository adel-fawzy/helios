#pragma once

#include <chrono>
#include <functional>
#include <sstream>
#include <string>

#include "log_level.hpp"

namespace helios::logging {

/**
 * @class logging::LogMessage
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
class LogMessage {
public:
  /**
   * @brief Type alias for the callback which is called when the message is
   *        finished.
   */
  using Callback = std::function<void(const std::string &)>;

  /**
   * @brief Constructor.
   */
  LogMessage(LogLevel level, std::string tag, Callback cb);

  /**
   * @brief Destructor.
   */
  ~LogMessage();

  /**
   * @brief Overloaded operator<<.
   *
   * @tparam MessageType Type of message to be logged.
   */
  template <typename MessageType>
  LogMessage &operator<<(const MessageType &value) {
    buffer_ << value;
    return *this;
  }

private:
  /**
   * @brief Notifies the observer that the message is finished.
   */
  void notify();

  /**
   * @brief Log level.
   */
  LogLevel level_;

  /**
   * @brief Tag.
   */
  std::string tag_;

  /**
   * @brief Observer callback.
   */
  Callback callback_;

  /**
   * @brief Time stamp when the object was created.
   */
  std::chrono::system_clock::time_point timestamp_;

  /**
   * @brief Buffer to hold the log message in between calls.
   */
  std::ostringstream buffer_;
}; // class LogMessage

} // namespace helios::logging
