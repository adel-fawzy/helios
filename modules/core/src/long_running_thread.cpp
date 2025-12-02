#include "long_running_thread.hpp"

#include <future>
#include <iostream>

namespace helios::core {
LongRunningThread::LongRunningThread(WorkFunc w, PredicateFunc p)
    : w_{w}, p_{p} {
  std::promise<void> started;
  auto loop = [this, &started] {
    started.set_value(); // Indicate that the function has started
    std::unique_lock<std::mutex> lock(mtx_);
    while (true) {
      cv_.wait(lock, [this] {
        std::cout << "LRT: Thread woke up to check client's predicate" << std::endl;
        auto result_1 = p_();
        std::cout << "Check client's predicate: " << result_1 << " and stop: " << stop_ << std::endl;
        return result_1 || stop_;
      }); // Sleep until there is work to do or to stop
      if (stop_) {
        // Stop is issued
        break; // Exit thread
      }
      // Release the lock while running the client's work
      lock.unlock();
      try {
        w_();
      } catch (const std::exception &e) {
        // TODO: Log e.what()
      } catch (...) {
        // TODO: Log an error
      }
      lock.lock();
    }
  };
  t_ = std::thread(loop); // Start thread
  // Wait until the thread is started
  auto fut = started.get_future();
  fut.wait();
}

LongRunningThread::~LongRunningThread() {
  std::cout << "LRT: Destructing lrt" << std::endl;
  {
    std::lock_guard<std::mutex> lock(mtx_);
    stop_ = true; // Indicate that the thread should be stopped
  }
  notify();  // Wake the thread in case it is sleeping
  t_.join(); // Wait for the thread to exit
  std::cout << "LRT: Destructed long running thread" << std::endl;
}

void LongRunningThread::notify() {
  std::cout << "LRT::notify(): waking up thread" << std::endl;
  cv_.notify_one();
  std::cout << "LRT::notify(): Woke up thread" << std::endl;
}
}; // namespace helios::core
