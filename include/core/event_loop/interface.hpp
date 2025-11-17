#pragma once

namespace helios::core::event_loop {

/**
 * @class event_loop::Interface
 *
 * @brief Starts an event loop in a separate thread and stops it.
 *
 * - The start() method is non-blocking and starts the event loop in a separate
 * thread.
 * - The stop() method blocks until the event loop thread has finished.
 */
class Interface {
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~Interface() = default;

  /**
   * @brief Starts the event loop.
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
}; // class Interface

} // namespace helios::core::event_loop
