#pragma once

#include <memory>
#include <string>

#include "logging/ilogger.hpp"

namespace helios::logging {

/**
 * @class logging::ILoggerFactory
 *
 * @brief Creates 'ILogger' to be used for logging.
 *
 * @details
 * - All public functions are thread-safe.
 * - All public functions are synchronous.
 */
class ILoggerFactory {
public:
  /**
   * @brief Default constructor
   */
  ILoggerFactory() = default;

  /**
   * @brief Default destructor
   */
  virtual ~ILoggerFactory() = default;

  /**
   * @brief Default copy semantics
   */
  ILoggerFactory(const ILoggerFactory &) = default;
  ILoggerFactory &operator=(const ILoggerFactory &) = default;

  /**
   * @brief Default move semantics
   */
  ILoggerFactory(ILoggerFactory &&) = default;
  ILoggerFactory &operator=(ILoggerFactory &&) = default;

  /**
   * @brief Creates 'ILogger' and then returns it.
   *
   * @param name The name of 'ILogger'.
   * 
   * @return Unique pointer to 'ILogger'.
   */
  virtual std::unique_ptr<ILogger> create(std::string name) = 0;
}; // class ILoggerFactory

} // namespace helios::logging
