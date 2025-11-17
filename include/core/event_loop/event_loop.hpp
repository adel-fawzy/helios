#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "core/event_queue/interface.hpp"
#include "interface.hpp"

namespace helios::core::event_loop {
/**
 * @class event_loop::EventLoop
 *
 * @brief Concrete implementation of the event loop interface.
 *
 * - When 'EventLoop::start' is called, it repeatedly calls 'event_queue::Interface::handle' in a separate thread.
 * - When 'EventLoop::stop' is called, it calls 'event_queue::Interface::push' to push a stop event.
 * - The destructor shall stop the event loop if it is running.
 */
class EventLoop : public Interface {
public:
  /**
   * @brief Constructor.
   */
  EventLoop(std::shared_ptr<event_queue::Interface> eventQueue);

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
  std::shared_ptr<event_queue::Interface> _eventQueue;

  /**
   * @brief Thread running the event loop.
   */
  std::thread _thread;

  /**
   * @brief Flag indicating if the event queue is stopped.
   */
  std::atomic<bool> _isQueueStopped{false};
};

} // namespace helios::core::event_loop
