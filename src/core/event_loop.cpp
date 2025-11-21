#include "event_loop.hpp"

namespace helios::core {

EventLoop::EventLoop(std::shared_ptr<IEventQueue> eventQueue)
    : eventQueue_(std::move(eventQueue)) {}

EventLoop::~EventLoop() { EventLoop::stop(); }

void EventLoop::start() {
  if (thread_.joinable()) {
    return; // Event loop already running
  }
  thread_ = std::thread([this]() {
    while (!isQueueStopped_) {
      try {
        eventQueue_->handle();
      } catch (...) {
        // TODO: Log an error: "Unknown exception occurred while handling event
        // in event loop"
      }
    }
  });
}

void EventLoop::stop() {
  if (!thread_.joinable()) {
    return; // Event loop not running
  }
  eventQueue_->push([this] {
    isQueueStopped_ = true;
  });                      // Signal that the event queue is stopped
  thread_.join();          // Wait for the thread to finish
  isQueueStopped_ = false; // Reset the flag for future starts
}

} // namespace helios::core
