#include "event_queue.hpp"

namespace helios::core {

void EventQueue::post(Event event) {
  std::lock_guard<std::mutex> lock(mtx_);
  queue_.push(std::move(event));
  cv_.notify_one();
}

bool EventQueue::tryPopAndExecute() {
  std::function<void()> f;
  {
    std::lock_guard<std::mutex> lock(mtx_);
    if (queue_.empty())
      return false;
    f = std::move(queue_.front());
    queue_.pop();
  }
  f();
  return true;
}

bool EventQueue::empty() { return queue_.empty(); }

} // namespace helios::core
