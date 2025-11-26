#include "long_running_thread.hpp"

namespace helios::core {

LongRunningThread::~LongRunningThread() { stop(); }

void LongRunningThread::start(std::function<void()> work,
                              std::function<bool()> predicate) {
  std::lock_guard<std::mutex> lock(mtx_);
  if (t_.joinable())
    // Thread is already running
    return;

  // Start thread
  std::promise<void> started;
  auto fut = started.get_future();
  t_ = std::thread([this, work = std::move(work),
                    predicate = std::move(predicate),
                    started = std::move(started)]() mutable {
    loop(std::move(work), std::move(predicate), std::move(started));
  });
  fut.wait(); // Wait until loop() has actually started
}

void LongRunningThread::stop() {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!t_.joinable())
      // Thread is already not running
      return;

    // Stop the thread
    stopCommand_.store(true, std::memory_order_release);
    cv_.notify_one(); // Wake the thread in case it is sleeping
  }

  // Wait for the thread function to return
  t_.join();

  // Reset stop command
  stopCommand_.store(false, std::memory_order_relaxed);
}

void LongRunningThread::notify() {
  std::lock_guard<std::mutex> lock(mtx_);
  cv_.notify_one();
}

void LongRunningThread::loop(std::function<void()> work,
                             std::function<bool()> predicate,
                             std::promise<void> promise) {
  promise.set_value(); // Indicate that the function has started
  std::unique_lock<std::mutex> lock(mtx_);
  while (!stopCommand_.load(std::memory_order_acquire)) {
    // Sleep until the client needs work or a stop is issued
    cv_.wait(lock, [this, predicate] {
      return stopCommand_.load(std::memory_order_acquire) || predicate();
    });
    // Check if the thread is woken up due to a stop command
    if (stopCommand_.load(std::memory_order_acquire)) {
      break; // Exit thread
    }
    // Release the lock while running the client's work
    lock.unlock();
    work();
    lock.lock();
  }
}

}; // namespace helios::core
