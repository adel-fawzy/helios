#pragma once

#include <unordered_map>
#include <vector>

#include "core/ievent_queue.hpp"
#include "logging/ilogger_factory.hpp"
#include "logging/isink.hpp"

namespace helios::logging {

/**
 * @class DefaultLoggerFactory
 *
 * @brief Creates 'ILogger'.
 *
 * @details
 * - Factory for creating a unique pointer to 'ILogger'.
 * - All public functions are not thread-safe.
 * - All public functions are synchronous.
 * - This class has a singleton object.
 */
class DefaultLoggerFactory : public ILoggerFactory {
public:
  /**
   * @brief Default constructor.
   */
  DefaultLoggerFactory();

  /**
   * @brief Default destructor.
   */
  ~DefaultLoggerFactory() override = default;

  /**
   * @brief Creates and returns the singleton instance.
   *
   * @return The singleton object
   */
  static DefaultLoggerFactory &instance();

  /**
   * @brief Creates 'ILogger' and then returns it.
   *
   * @param name The name of 'ILogger'.
   *
   * @return Unique pointer to 'ILogger'.
   */
  std::unique_ptr<ILogger> create(std::string name) override;

  /**
   * @brief Returns the event queue.
   */
  std::shared_ptr<core::IEventQueue> getEventQueue() const;

private:
  /**
   * @brief Event queue used for logging.
   */
  std::shared_ptr<core::IEventQueue> eventQueue_;

  /**
   * @brief Shared pointer to the standard output logging sink.
   */
  std::shared_ptr<ISink> standardOutSink_;

  /**
   * @brief Shared pointer to the file logging sink.
   */
  std::shared_ptr<ISink> fileSink_;
}; // class DefaultLoggerFactory

} // namespace helios::logging
