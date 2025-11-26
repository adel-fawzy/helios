#include "core/event_loop.hpp"

#include "long_running_thread.hpp"

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>
#include <vector>

namespace helios::core {

class EventLoop::Impl {
public:
  Impl() = default;
  ~Impl();
  void add(std::shared_ptr<HObject> obj);
  void add(std::initializer_list<std::shared_ptr<HObject>> objs);

  void run();
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
  LongRunningThread t_;

  /**
   * @brief Set to true when a stop is issued. Reset after the thread is
   *        stopped.
   */
  std::atomic<bool> isStopIssued_{false};

  /**
   * @brief Keeps on looping until all events from all HObjects are handled.
   */
  void handleEvents();

  /**
   * @brief Loops on all current HObjects and then executes one event from each.
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
   * @brief Adds stop events to all HObjects and then waits for them to be
   *        executed.
   *
   * @param snapshot Current available HObjects.
   */
  void flushQueues(std::vector<std::shared_ptr<HObject>> &snapshot);

  /**
   * @brief Unsubscribes from all HObjects.
   */
  void unsubscribe();
}; // class EventLoop::Impl

EventLoop::EventLoop() : impl_{std::make_unique<Impl>()} {}

EventLoop::~EventLoop() = default;

EventLoop::Impl::~Impl() {
  stop();
  unsubscribe();
}

void EventLoop::add(std::shared_ptr<HObject> obj) { impl_->add(obj); }

void EventLoop::add(std::initializer_list<std::shared_ptr<HObject>> objs) {
  impl_->add(std::move(objs));
}

void EventLoop::run() { impl_->run(); }

void EventLoop::stop() { impl_->stop(); }

void EventLoop::Impl::add(std::shared_ptr<HObject> obj) {
  objects_.push_back(obj);
  obj->subscribe([this] { t_.notify(); });
}

void EventLoop::Impl::add(
    std::initializer_list<std::shared_ptr<HObject>> objs) {
  for (auto &obj : objs) {
    add(obj);
  }
}

void EventLoop::Impl::run() {
  // Start thread if not already started
  t_.start(
      // WORK
      [this] { handleEvents(); },
      // PREDICATE
      [this] {
        std::lock_guard<std::mutex> lock(mtx_);
        return anyQueueHasWork();
      });
}

void EventLoop::Impl::stop() {
  std::vector<std::shared_ptr<HObject>> snapshot = collectAliveObjects();
  flushQueues(snapshot);
  t_.stop();
}

void EventLoop::Impl::handleEvents() {
  while (dispatchOnce()) {
    // Loop until all events from all HObjects are handled
  }
}

bool EventLoop::Impl::dispatchOnce() {
  std::vector<std::shared_ptr<HObject>> snapshot = collectAliveObjects();

  bool didWork{false};

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

bool EventLoop::Impl::anyQueueHasWork() {
  return std::any_of(objects_.begin(), objects_.end(), [](auto &w) {
    if (auto s = w.lock())
      return s->hasEvents();
    return false;
  });
}

void EventLoop::Impl::flushQueues(
    std::vector<std::shared_ptr<HObject>> &snapshot) {
  std::vector<std::future<void>> futures;
  futures.reserve(snapshot.size());
  for (auto &obj : snapshot) {
    auto p = std::make_shared<std::promise<void>>();
    futures.push_back(p->get_future());
    auto stopEvent = [this, p]() mutable { p->set_value(); };
    obj->post(std::move(stopEvent)); // Add a stop event to the HObject
  }
  cv_.notify_one(); // Wake loop thread in case it is sleeping
  // Wait for all HObjects stop events to be executed
  for (auto &f : futures) {
    f.get();
  }
}

void EventLoop::Impl::unsubscribe() {
  std::vector<std::shared_ptr<HObject>> snapshot = collectAliveObjects();
  for (auto &obj : snapshot)
    obj->unsubscribe();
}

} // namespace helios::core
