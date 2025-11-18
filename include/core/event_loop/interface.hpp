#pragma once

namespace helios::core::event_loop {

/**
 * @class event_loop::Interface
 *
 * @brief Starts an event loop in a separate thread and stops it.
 *
 * - All public functions are not thread-safe because the event loop should be
 *   called from one thread only.
 */
class Interface {
public:
  /**
   * @brief Default constructor.
   */
  Interface() = default;

  /**
   * @brief Virtual destructor.
   */
  virtual ~Interface() = default;

  /**
   * @brief Delete copy semantics.
   */
  Interface(const Interface &) = delete;
  Interface &operator=(const Interface &) = delete;

  /**
   * @brief Default move semantics.
   */
  Interface(Interface &&) = default;
  Interface &operator=(Interface &&) = default;

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
}; // class Interface

} // namespace helios::core::event_loop
