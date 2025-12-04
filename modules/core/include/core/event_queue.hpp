#pragma once

#include <mutex>
#include <queue>

#include "event.hpp"

namespace helios::core {

/**
 * @class core::EventQueue
 *
 * @brief Stores events in a thread-safe queue.
 *
 * @note
 * - All public functions are thread-safe.
 * - All public functions are synchronous.
 */
class EventQueue {
public:
  /**
   * @brief Default constructor.
   */
  EventQueue() = default;

  /**
   * @brief Default destructor.
   */
  ~EventQueue() = default;

  /**
   * @brief Delete copy and move semantics.
   */
  EventQueue(const EventQueue &) = delete;
  EventQueue &operator=(const EventQueue &) = delete;
  EventQueue(EventQueue &&) = delete;
  EventQueue &operator=(EventQueue &&) = delete;

  /**
   * @brief Adds an event to the queue.
   *
   * @tparam EventT Type of Event to be added.
   * @param event Event to be added.
   */
  template <typename EventT> void post(EventT &&event) {
    std::lock_guard<std::mutex> lock(mtx_);
    q_.push(std::forward<EventT>(event));
  }

  /**
   * @brief Handles the first event in the queue and then returns.
   *
   * @return True if an event is handled, false otherwise.
   */
  bool tryPopAndExecute();

  /**
   * @brief Checks if the queue is empty.
   *
   * @return Returns true if the queue is empty, false otherwise.
   */
  bool empty() const;

private:
  /**
   * @brief Holds events to be handled later.
   */
  std::queue<Event> q_;

  /**
   * @brief Protects this class.
   */
  std::mutex mtx_;
}; // class EventQueue

} // namespace helios::core
