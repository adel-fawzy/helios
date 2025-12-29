#include "timesys/timers_manager.hpp"

#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "core/active_h_object.hpp"

namespace helios::timesys {

class TimersManager::Impl : public core::ActiveHObject {
public:
  /**
   * @brief Constructor.
   */
  Impl();

  /**
   * @brief Destructor.
   */
  ~Impl();

  /**
   * @brief Struct that represents a timer.
   */
  struct Timer {
    Timer(TimePoint tp, Callback cb) : tp_{tp}, cb_{cb} {}
    TimePoint tp_;
    Callback cb_;
    bool operator<(const Timer &other) const { return tp_ > other.tp_; }
  }; // struct Timer

  /**
   * @brief Queue of active timers.
   */
  std::priority_queue<Timer> q_;

  /**
   * @brief Timer thread.
   */
  std::thread t_;

  /**
   * @brief Protects the class.
   */
  std::mutex mtx_;

  /**
   * @brief Wakes the thread.
   */
  std::condition_variable cv_;

  /**
   * @brief Set to true to stop the thread, false otherwise.
   */
  bool stopRunning_{false};

  /**
   * @brief Main thread function.
   */
  void run();
}; // class Impl

TimersManager::TimersManager()
    : impl_{std::make_unique<TimersManager::Impl>()} {}

TimersManager::~TimersManager() = default;

void TimersManager::create(Duration duration, Callback cb) {
  {
    std::lock_guard<std::mutex> lock(impl_->mtx_);
    impl_->q_.emplace(
        duration + std::chrono::steady_clock::now(), std::move(cb)
    );
  }
  impl_->cv_.notify_one(); // Wake the thread
}

TimersManager::Impl::Impl() {
  std::promise<void> started;
  auto main = [this, &started] {
    started.set_value(); // Indicate that the loop thread started
    run();               // Run event queue
  };
  t_ = std::thread(main);     // Start loop thread
  started.get_future().get(); // Wait for loop thread to start
}

TimersManager::Impl::~Impl() {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    stopRunning_ = true; // Stop the thread
  }
  cv_.notify_one(); // Wake the thread
  t_.join();        // Wait for loop thread to exit
}

void TimersManager::Impl::run() {
  while (true) {
    std::unique_lock<std::mutex> lock(mtx_);

    if (stopRunning_)
      break;

    if (q_.empty()) {
      cv_.wait(lock, [this] { return stopRunning_ || !q_.empty(); });
      if (stopRunning_)
        break;
    }

    Timer top = q_.top();
    auto now = SteadyClock::now();

    if (top.tp_ <= now) {
      q_.pop();
      lock.unlock();
      top.cb_();
      continue;
    }

    cv_.wait_until(lock, top.tp_, [this, &top] {
      return stopRunning_ || q_.top().tp_ < top.tp_;
    });
  }
};

} // namespace helios::timesys
