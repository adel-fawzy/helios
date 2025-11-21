#pragma once

#include <functional>
#include <queue>

#include "core/event.hpp"

namespace helios::core {

/**
 * @class core::IEventQueue
 *
 * @brief Thread-safe event queue.
 *
 * @details
 * - Holds and handles events for event-driven classes.
 * - The event is a callable object. It takes no parameters and
 *   returns void.
 * - All public functions are thread-safe.
 * - All public functions are synchronous.
 */
class IEventQueue {
public:
  /**
   * @brief Default constructor.
   */
  IEventQueue() = default;

  /**
   * @brief Default virtual destructor.
   */
  virtual ~IEventQueue() = default;

  /**
   * @brief Delete copy semantics.
   */
  IEventQueue(const IEventQueue &) = delete;
  IEventQueue &operator=(const IEventQueue &) = delete;

  /**
   * @brief Default move semantics.
   */
  IEventQueue(IEventQueue &&) = default;
  IEventQueue &operator=(IEventQueue &&) = default;

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
  virtual void pushImpl(Event event) = 0;
}; // class IEventQueue

} // namespace helios::core
