#include "DefaultEventQueue.hpp"

namespace helios::EventQueue {

void DefaultEventQueue::push(Event event) {
  std::lock_guard<std::mutex> lock(_mtx);
  _queue.push(event);
  _cv.notify_one();
}

void DefaultEventQueue::handle() {
  std::unique_lock<std::mutex> lock(_mtx);
  // Sleep until an event is available
  _cv.wait(lock, [this] { return !_queue.empty(); });
  Event event = std::move(_queue.front()); // Move the event
  _queue.pop();                            // Pop the event from the queue
  event();                                 // Execute the event
}

} // namespace helios::EventQueue
