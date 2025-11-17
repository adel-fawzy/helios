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
 * - This queue holds events for event-driven classes and can handle them. These
 *   events should be callable objects where they take no parameters and return
 *   void.
 * - All public functions are thread-safe.
 */
class Interface {
public:
  /**
   * @brief Type alias for the queue of events.
   */
  using QueueOfEvents = std::queue<Event>;

  /**
   * @brief Virtual destructor.
   */
  virtual ~Interface() = default;

  /**
   * @brief Pushes an event to the event queue.
   *
   * @param event Event.
   */
  virtual void push(Event event) = 0;

  /**
   * @brief Handles an event from the event queue.
   *
   * In case there are no events in the queue, this function will block (let the
   * thread sleep) until an event is available.
   */
  virtual void handle() = 0;
}; // class Interface
} // namespace helios::core::event_queue
