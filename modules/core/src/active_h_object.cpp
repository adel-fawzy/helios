#include "core/active_h_object.hpp"

#include <atomic>
#include <thread>

namespace helios::core {

ActiveHObject::ActiveHObject(std::shared_ptr<SignalBus> signalBus)
    : HObject(std::move(signalBus)), t_{} {
  std::promise<void> pr;
  std::future<void> fut = pr.get_future();
  auto main = [this, &pr]() mutable {
    pr.set_value(); // Indicate that the thread started
    run();
  };
  t_ = std::thread(main); // Start thread
  fut.get();              // Wait for the thread to start
}

ActiveHObject::~ActiveHObject() {
  std::cout << "ActiveHObject::~ActiveHObject: Called" << std::endl;
  shutdown(); // Shut down the event queue
  std::cout << "ActiveHObject::~ActiveHObject: Called shutdown()" << std::endl;
  {
    std::lock_guard<std::mutex> lock(mtx_);
    stopRunning_ = true; // Stop the thread
  }
  cv_.notify_one(); // Wake the thread in case it is sleeping
  std::cout << "ActiveHObject::~ActiveHObject: Notified thread" << std::endl;
  t_.join(); // Wait for the thread to exit
  std::cout << "ActiveHObject::~ActiveHObject: Thread exited" << std::endl;
}

void ActiveHObject::onEventPosted() {}

void ActiveHObject::run() {
  std::cout << "ActiveHObject::run: Called" << std::endl;
  while (true) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] { return stopRunning_ || !q_.empty(); });

    if (stopRunning_)
      break;

    // Extract all events into a local queue
    std::deque<Event> local;
    local.swap(q_);

    // Execute them without holding the mutex
    lock.unlock();
    for (auto &e : local)
      handle(e);
  }
}

}; // namespace helios::core
