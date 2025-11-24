#include "core/event_loop.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace helios::core {

class EventLoop::Impl {
public:
  void add(std::shared_ptr<HObject> obj);
  void start();
  void stop();

private:
  /**
   * @brief Protects this class.
   */
  std::mutex mtx_;

  /**
   * @brief Indicates to the event loop that it must wake due to a new event or
   * a stop event.
   */
  std::condition_variable cv_;

  /**
   * @brief Holds the HObjects that their events are handled.
   */
  std::vector<std::weak_ptr<HObject>> objects_;

  /**
   * @brief Event loop thread.
   */
  std::thread thread_;

  /**
   * @brief Set to true when the thread is running. Set to false to stop the
   *        thread.
   */
  std::atomic<bool> running_{false};

  /**
   * @brief Main function that runs the loop.
   */
  void runLoop();

  /**
   * @brief Executes one event from each HObject.
   *
   * @return True if at least one event is executed. False if no events were
   *         executed from any HObject.
   */
  bool dispatchOnce();

  /**
   * @brief Creates a snapshot of the available HObjects.
   *
   * @return Shared pointers to the available HObjects so they could be
   *         executed.
   */
  std::vector<std::shared_ptr<HObject>> collectAliveObjects();

  /**
   * @brief Sleeps the event loop thread until a new event is added for any
   *        HObject or the event loop is stopped.
   */
  void waitForEvents();

  /**
   * @brief Checks if any HObject has at least one event.
   *
   * @return True if at least one HObject has at least one event. False
   *         otherwise.
   */
  bool anyQueueHasWork();
}; // class EventLoop::Impl

EventLoop::EventLoop() : impl_{std::make_unique<Impl>()} {}

EventLoop::~EventLoop() { impl_->stop(); }

void EventLoop::add(std::shared_ptr<HObject> obj) { impl_->add(obj); }

void EventLoop::start() { impl_->start(); }

void EventLoop::stop() { impl_->stop(); }

void EventLoop::Impl::add(std::shared_ptr<HObject> obj) {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    objects_.push_back(obj);
  }
  cv_.notify_one();
}

void EventLoop::Impl::start() {
  bool expected = false;
  if (running_.compare_exchange_strong(expected, true)) {
    thread_ = std::thread([this] { runLoop(); });
  }
}

void EventLoop::Impl::stop() {
  running_ = false;
  cv_.notify_one();
  if (thread_.joinable())
    thread_.join();
}

void EventLoop::Impl::runLoop() {
  while (running_) {
    bool didWork = dispatchOnce();

    if (!didWork)
      waitForEvents();
  }
}

bool EventLoop::Impl::dispatchOnce() {
  auto snapshot = collectAliveObjects();

  bool didWork = false;
  for (auto &obj : snapshot) {
    if (obj->tryPopAndExecute())
      didWork = true;
  }

  return didWork;
}

std::vector<std::shared_ptr<HObject>> EventLoop::Impl::collectAliveObjects() {
  std::vector<std::shared_ptr<HObject>> snapshot;

  std::lock_guard<std::mutex> lock(mtx_);

  // remove destroyed objects
  objects_.erase(std::remove_if(objects_.begin(), objects_.end(),
                                [](auto &w) { return w.expired(); }),
                 objects_.end());

  // lock valid ones
  for (auto &w : objects_) {
    if (auto s = w.lock())
      snapshot.push_back(s);
  }

  return snapshot;
}

void EventLoop::Impl::waitForEvents() {
  std::unique_lock<std::mutex> lock(mtx_);
  cv_.wait(lock, [this] { return !running_ || anyQueueHasWork(); });
}

bool EventLoop::Impl::anyQueueHasWork() {
  return std::any_of(objects_.begin(), objects_.end(), [](auto &w) {
    if (auto s = w.lock())
      return s->hasEvents();
    return false;
  });
}

} // namespace helios::core
