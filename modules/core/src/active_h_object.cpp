#include "core/active_h_object.hpp"

#include <future>

namespace helios::core {

ActiveHObject::ActiveHObject(std::shared_ptr<HBus> hBus)
    : HObject(std::move(hBus)) {
  std::promise<void> started;
  auto main = [this, &started] {
    started.set_value(); // Indicate that the loop thread started
    run();               // Run event queue
  };
  t_ = std::thread(main);     // Start loop thread
  started.get_future().get(); // Wait for loop thread to start
}

ActiveHObject::~ActiveHObject() {
  // Post a stop event
  std::promise<void> finished;
  postImpl([this, &finished] {
    finished.set_value(); // Indicate that the event has executed
  });
  finished.get_future().get(); // Wait for the stop event to be executed

  {
    std::lock_guard<std::mutex> lock(mtx_);
    stopLoop_ = true; // Stop the loop thread
  }
  
  cv_.notify_one(); // Notify loop thread
  t_.join();        // Wait for loop thread to exit
}

void ActiveHObject::postImpl(Event e) {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    q_.emplace_back(std::move(e));
  }
  cv_.notify_one(); // Notify loop thread
}

void ActiveHObject::run() {
  while (true) {
    std::deque<Event> snapshot;
    {
      std::unique_lock<std::mutex> lock(mtx_);

      // Sleep until the thread is stopped or an event is added to the queue
      cv_.wait(lock, [this] { return stopLoop_ || !q_.empty(); });

      if (stopLoop_)
        break;
      snapshot.swap(q_); // Take a snapshot of the queue
    }
    // Handle events in the snapshot
    for (const auto &event : snapshot) {
      try {
        event(); // Handle event
      } catch (const std::exception &e) {
        // TODO: Log
      } catch (...) {
        // TODO: Log
      }
    }
  }
}

} // namespace helios::core
