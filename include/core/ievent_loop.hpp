#pragma once

namespace helios::core {

/**
 * @class core::IEventLoop
 *
 * @brief Starts an event loop in a separate thread and stops it.
 *
 * @details
 * - All public functions are not thread-safe because the event loop should be
 *   called from one thread only.
 * - All public functions are synchronous.
 */
class IEventLoop {
public:
  /**
   * @brief Default constructor.
   */
  IEventLoop() = default;

  /**
   * @brief Virtual destructor.
   */
  virtual ~IEventLoop() = default;

  /**
   * @brief Delete copy semantics.
   */
  IEventLoop(const IEventLoop &) = delete;
  IEventLoop &operator=(const IEventLoop &) = delete;

  /**
   * @brief Default move semantics.
   */
  IEventLoop(IEventLoop &&) = default;
  IEventLoop &operator=(IEventLoop &&) = default;

  /**
   * @brief Starts the event loop in a separate thread.
   *
   * - If called when the event loop is already running, this function does
   *   nothing.
   */
  virtual void start() = 0;

  /**
   * @brief Stops the event loop.
   *
   * - If called when the event loop is not running, this function does nothing.
   * - Will block the calling thread until the event loop thread has finished.
   */
  virtual void stop() = 0;
}; // class IEventLoop

} // namespace helios::core::event_loop
