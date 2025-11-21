#include "event_queue.hpp"

namespace helios::core {

void EventQueue::pushImpl(Event event) {
  std::lock_guard<std::mutex> lock(mtx_);
  queue_.push(std::move(event));
  cv_.notify_one();
}

void EventQueue::handle() {
  std::unique_lock<std::mutex> lock(mtx_);
  // Sleep until an event is available
  cv_.wait(lock, [this] { return !queue_.empty(); });
  Event event = std::move(queue_.front()); // Move the event
  queue_.pop();                            // Pop the event from the queue
  event();                                 // Execute the event
}

} // namespace helios::core
