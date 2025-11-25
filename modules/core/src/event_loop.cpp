#include "core/event_loop.hpp"

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>
#include <vector>
#include <iostream>
namespace helios::core {

class EventLoop::Impl {
public:
  void add(std::shared_ptr<HObject> obj);
  void start();
  void stopAndWait();

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
   * @brief Set to true when the loop is stopped.
   */
  std::atomic<bool> stopping_{false};

  /**
   * @brief Main function that runs the loop.
   *
   * @param p Promise used to indicate to the thread that started the event loop
   *          that the thread has started successfully.
   */
  void runLoop(std::promise<void> &p);

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

  /**
   * @brief Flushes all HObjects that have events that were posted before
   * stopAndWait() was called.
   */
  void flushQueues();
}; // class EventLoop::Impl

EventLoop::EventLoop() : impl_{std::make_unique<Impl>()} {}

EventLoop::~EventLoop() { impl_->stopAndWait(); }

void EventLoop::add(std::shared_ptr<HObject> obj) { impl_->add(obj); }

void EventLoop::start() { impl_->start(); }

void EventLoop::stopAndWait() { impl_->stopAndWait(); }

void EventLoop::Impl::add(std::shared_ptr<HObject> obj) {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    objects_.push_back(obj);
    obj->subscribe([this] { cv_.notify_one(); });
  }
  cv_.notify_one();
}

void EventLoop::Impl::start() {
  bool expected = false;
  if (running_.compare_exchange_strong(expected, true)) {
    std::promise<void> p;
    std::future<void> f = p.get_future();
    thread_ = std::thread([this, p = std::move(p)]() mutable { runLoop(p); });
    f.get(); // Wait for the thread of the event loop to start
  }
}

void EventLoop::Impl::stopAndWait() {
  if (!running_) {
    std::cout << "event loop is not running" << std::endl;
    return;
  }
  std::cout << "event loop is running" << std::endl;
  bool expected = false;
  if (!stopping_.compare_exchange_strong(expected, true)) {
    // Stop already in progress or done
    if (thread_.joinable()) {
      if (running_.load(std::memory_order_acquire)) {
        cv_.notify_one();
      }
      thread_.join();
    }
    return;
  }
  std::cout << "Flushing queues" << std::endl;
  flushQueues();
std::cout << "Flushed queues" << std::endl;
  running_.store(false, std::memory_order_release); // Stop the thread

  cv_.notify_one(); // Notify the thread in case it is sleeping

  if (thread_.joinable()) {
    // std::cout << "Joining thread" << std::endl;
    thread_.join();
    // std::cout << "Joined thread" << std::endl;
  }

  stopping_ = false; // Reset
}

void EventLoop::Impl::runLoop(std::promise<void> &p) {
  p.set_value();

  while (running_) {
    bool didWork = dispatchOnce();

    if (!didWork)
      waitForEvents();
  }
}

bool EventLoop::Impl::dispatchOnce() {
  std::vector<std::shared_ptr<HObject>> snapshot = collectAliveObjects();

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

void EventLoop::Impl::flushQueues() {
  std::vector<std::shared_ptr<HObject>> snapshot = collectAliveObjects();
  std::vector<std::future<void>> futures;
  futures.reserve(snapshot.size());
  for (auto &obj : snapshot) {
    auto p = std::make_shared<std::promise<void>>();
    futures.push_back(p->get_future());
    auto stopEvent = [this, p]() mutable {
      p->set_value();
    };
    obj->post(std::move(stopEvent)); // Add a stop event to the HObject
  }
  cv_.notify_one(); // Wake loop thread in case it is sleeping
  // Wait for all HObjects stop events to be executed
  for (auto &f : futures) {
    f.get();
  }
  for (auto &obj : snapshot) {
    obj->unsubscribe();
  }
}

} // namespace helios::core
