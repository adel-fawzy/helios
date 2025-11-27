#pragma once

#include <string>

namespace helios::logger {

/**
 * @brief Represents a log message.
 */
struct LogMessage {
  /**
   * @brief The message.
   */
  std::string msg;

  /**
   * @brief Constructor.
   */
  template <typename S> LogMessage(S &&s) : msg(std::forward<S>(s)) {}
}; // struct LogMessage

} // namespace helios::logger
