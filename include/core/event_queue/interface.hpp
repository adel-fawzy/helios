#pragma once

#include <functional>
#include <queue>

#include "core/types/event.hpp"

namespace helios::core::event_queue {

/**
 * @class event_queue::Interface
 *
 * @brief Thread-safe event queue.
 *
 * - Holds and handles events for event-driven classes.
 * - The event is a callable object. It takes no parameters and
 *   returns void.
 * - All public functions in this clasee are thread-safe.
 */
class Interface {
public:
  /**
   * @brief Default constructor.
   */
  Interface() = default;

  /**
   * @brief Default virtual destructor.
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
   * @brief Pushes an event to the event queue.
   *
   * @param event Event.
   */
  template <typename EventType> void push(EventType &&event) {
    pushImpl(std::forward<EventType>(event));
  }

  /**
   * @brief Handles the next event in the queue and then returns.
   *
   * - In case there are no events in the queue, this function will block (let
   *   the thread sleep) until an event is available.
   */
  virtual void handle() = 0;

protected:
  /**
   * @brief Pushes an event to the event queue (implementation).
   */
  virtual void pushImpl(const Event &event) = 0;
}; // class Interface

} // namespace helios::core::event_queue
