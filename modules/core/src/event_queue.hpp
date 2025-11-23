#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

#include "core/event.hpp"

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
   * @param event Event to be added.
   */
  void post(Event event);

  /**
   * @brief Handles the first event in the queue and then returns.
   *
   * @note
   * - Will silently return if the queue is empty.
   */
  bool tryPopAndExecute();

  /**
   * @brief Checks if the queue is empty.
   *
   * @return Returns true if the queue is empty, false otherwise.
   */
  bool empty();

private:
  /**
   * @brief Holds events to be handled later.
   */
  std::queue<Event> queue_;

  /**
   * @brief Mutex to protect this class.
   */
  std::mutex mtx_;

  /**
   * @brief Condition variable to signal when an event is available in the
   *        queue.
   */
  std::condition_variable cv_;
}; // class EventQueue

} // namespace helios::core
