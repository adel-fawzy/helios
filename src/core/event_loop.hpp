#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "core/ievent_queue.hpp"
#include "core/ievent_loop.hpp"

namespace helios::core {

/**
 * @class core::EventLoop
 *
 * @brief Concrete implementation of the event loop interface.
 *
 * @details
 * - When 'EventLoop::start' is called, it repeatedly calls 'event_queue::Interface::handle' in a separate thread.
 * - When 'EventLoop::stop' is called, it calls 'event_queue::Interface::push' to push a stop event.
 * - The destructor shall stop the event loop if it is running.
 */
class EventLoop : public IEventLoop {
public:
  /**
   * @brief Constructor.
   */
  EventLoop(std::shared_ptr<IEventQueue> eventQueue);

  /**
   * @brief Default destructor.
   */
  ~EventLoop() override;

  /**
   * @brief Delete copy and move semantics.
   */
  EventLoop(const EventLoop &) = delete;
  EventLoop &operator=(const EventLoop &) = delete;
  EventLoop(EventLoop &&) = delete;
  EventLoop &operator=(EventLoop &&) = delete;

  void start() override;

  void stop() override;

private:
  /**
   * @brief Shared pointer to the event queue.
   */
  std::shared_ptr<IEventQueue> eventQueue_;

  /**
   * @brief Thread running the event loop.
   */
  std::thread thread_;

  /**
   * @brief Flag indicating if the event queue is stopped.
   */
  std::atomic<bool> isQueueStopped_{false};
}; // class EventLoop

} // namespace helios::core
