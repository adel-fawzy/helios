#include "core/event_loop/event_loop.hpp"

namespace helios::core::event_loop {

EventLoop::EventLoop(std::shared_ptr<event_queue::Interface> eventQueue)
    : _eventQueue(std::move(eventQueue)) {}

EventLoop::~EventLoop() { EventLoop::stop(); }

void EventLoop::start() {
  if (_thread.joinable()) {
    return; // Event loop already running
  }
  _thread = std::thread([this]() {
    while (!_isQueueStopped) {
      try {
        _eventQueue->handle();
      } catch (...) {
        // TODO: Log an error: "Unknown exception occurred while handling event
        // in event loop"
      }
    }
  });
}

void EventLoop::stop() {
  if (!_thread.joinable()) {
    return; // Event loop not running
  }
  _eventQueue->push([this] {
    _isQueueStopped = true;
  });                      // Signal that the event queue is stopped
  _thread.join();          // Wait for the thread to finish
  _isQueueStopped = false; // Reset the flag for future starts
}

} // namespace helios::core::event_loop
