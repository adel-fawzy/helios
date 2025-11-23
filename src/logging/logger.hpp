#pragma once

#include <memory>
#include <string>
#include <vector>

#include "logging/ilogger.hpp"
#include "logging/isink.hpp"

namespace helios::logging {

/**
 * @class logging::Logger
 *
 * @brief Logs a message.
 *
 * @details
 * - Logs a message to a list of sinks (ISink).
 * - The public API returns 'LogMessage' which is used by the client to create
 *   the log message.
 * - During the construction of 'LogMessage', passes to it a callback which will
 *   be called by the 'LogMessage' to notify the 'Logger' that the log message
 *   is created.
 * - This callback shall log to the sinks (ISink).
 * - All public functions are not thread safe.
 * - All public functions are synchronous.
 */
class Logger : public ILogger {
public:
  /**
   * @brief Constructor.
   *
   * @param name The name used in the log message to indicate which module or
   *             class made the log.
   * @param sinks The list of sinks to log to.
   */
  Logger(std::string name, std::vector<std::shared_ptr<ISink>> sinks);

  /**
   * @brief Default destructor.
   */
  ~Logger() override = default;

  LogMessage debug() override;
  LogMessage info() override;
  LogMessage warn() override;
  LogMessage error() override;

private:
  /**
   * @brief Name of the logger.
   */
  std::string name_;

  /**
   * @brief Logging sinks.
   */
  std::vector<std::shared_ptr<ISink>> sinks_;

  /**
   * @brief Returns an empty LogMessage.
   */
  LogMessage make(LogLevel lvl);
}; // class Logger

} // namespace helios::logging
