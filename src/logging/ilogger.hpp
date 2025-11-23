#pragma once

#include "log_message.hpp"

namespace helios::logging {

/**
 * @class logging::ILogger
 *
 * @brief Logs a message to one or more logging sinks.
 *
 * @details
 * - All public functions are not thread-safe.
 * - All public functions are synchronous.
 */
class ILogger {
public:
  /**
   * @brief Default constructor
   */
  ILogger() = default;

  /**
   * @brief Default destructor
   */
  virtual ~ILogger() = default;

  /**
   * @brief Delete copy semantics
   */
  ILogger(const ILogger &) = delete;
  ILogger &operator=(const ILogger &) = delete;

  /**
   * @brief Default move semantics
   */
  ILogger(ILogger &&) = default;
  ILogger &operator=(ILogger &&) = default;

  /**
   * @brief Returns an empty debug log message.
   *
   * @return Empty debug LogMessage.
   */
  virtual LogMessage debug() = 0;

  /**
   * @brief Returns an empty info log message.
   *
   * @return Empty info LogMessage.
   */
  virtual LogMessage info() = 0;

  /**
   * @brief Returns an empty warn log message.
   *
   * @return Empty warn LogMessage.
   */
  virtual LogMessage warn() = 0;

  /**
   * @brief Returns an empty error log message.
   *
   * @return Empty error LogMessage.
   */
  virtual LogMessage error() = 0;
}; // class ILogger

} // namespace helios::logging
