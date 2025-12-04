#include "core/event_queue.hpp"

namespace helios::core {

bool EventQueue::tryPopAndExecute() {
  std::function<void()> e;
  {
    std::lock_guard<std::mutex> lock(mtx_);
    if (queue_.empty())
      // No events to handle
      return false;
    e = queue_.front();
    queue_.pop();
  }
  e(); // Handle event
  return true;
}

bool EventQueue::empty() { return queue_.empty(); }

} // namespace helios::core
