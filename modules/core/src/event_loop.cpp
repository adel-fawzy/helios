#include "core/event_loop.hpp"

#include "long_running_thread.hpp"

#include <future>
#include <iostream>
#include <mutex>
#include <vector>

namespace helios::core {

class EventLoop::Impl {
public:
  Impl(std::initializer_list<std::shared_ptr<HObject>> objs);
  ~Impl();

  /**
   * @brief Protects this class.
   */
  std::mutex mtx_;

  /**
   * @brief Holds the HObjects that their events are handled.
   */
  std::vector<std::weak_ptr<HObject>> objects_;

  /**
   * @brief Event loop thread.
   */
  LongRunningThread t_;

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
   * @brief Checks if any HObject has at least one event.
   *
   * @return True if at least one HObject has at least one event. False
   *         otherwise.
   */
  bool anyQueueHasWork();

  /**
   * @brief Adds stop events to all HObjects and then waits for them to be
   *        executed.
   */
  void flushQueues();

  /**
   * @brief Unsubscribes from all HObjects.
   */
  void unsubscribe();

  /**
   * @brief Subscribes to all HObjects.
   */
  void subscribe();
}; // class EventLoop::Impl

EventLoop::EventLoop()
    : impl_{std::make_unique<Impl>(
          std::initializer_list<std::shared_ptr<HObject>>{}
      )} {}

EventLoop::EventLoop(std::shared_ptr<HObject> obj)
    : EventLoop({std::move(obj)}) {}

EventLoop::EventLoop(std::initializer_list<std::shared_ptr<HObject>> objs)
    : impl_{std::make_unique<Impl>(objs)} {}

EventLoop::~EventLoop() { std::cout << "Destructing loop" << std::endl; }

void EventLoop::add(std::shared_ptr<HObject> obj) {
  std::cout << "add: Locking mutex" << std::endl;
  std::lock_guard<std::mutex> lock(impl_->mtx_);
  std::cout << "add: Subscribing" << std::endl;
  obj->subscribe([this] {
    std::cout << "eventloop::callback: Before call" << std::endl;
    impl_->t_.notify();
    std::cout << "eventloop::callback: After call" << std::endl;
  });
  std::cout << "add: Pushing back" << std::endl;
  impl_->objects_.push_back(obj);
  std::cout << "add: Notifying" << std::endl;
  impl_->t_.notify();
  std::cout << "add: exiting" << std::endl;
}

EventLoop::Impl::Impl(std::initializer_list<std::shared_ptr<HObject>> objs)
    : t_{// WORK
         [this] { handleEvents(); },
         // PREDICATE
         [this] { return anyQueueHasWork(); }
      } {
  objects_.assign(objs.begin(), objs.end());
  subscribe();
}

EventLoop::Impl::~Impl() {
  std::cout << "Flushing queue" << std::endl;
  flushQueues();
  std::cout << "Unsubscribing" << std::endl;
  unsubscribe();
  std::cout << "-------------------------Unsubscribed" << std::endl;
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

  // Remove destroyed objects
  objects_.erase(
      std::remove_if(
          objects_.begin(), objects_.end(), [](auto &w) { return w.expired(); }
      ),
      objects_.end()
  );

  // Lock valid ones
  for (auto &w : objects_) {
    if (auto s = w.lock())
      snapshot.push_back(s);
  }

  return snapshot;
}

bool EventLoop::Impl::anyQueueHasWork() {
  std::cout << "anyQueueHasWork() begin" << std::endl;
  std::vector<std::shared_ptr<HObject>> snapshot = collectAliveObjects();
  for (auto &obj : snapshot) {
    if (obj->hasEvents()) {
      std::cout << "anyQueueHasWork() ending" << std::endl;
      return true;
    }
  }
  std::cout << "anyQueueHasWork() ending" << std::endl;

  return false;
}

void EventLoop::Impl::flushQueues() {
  std::cout << "flushQueues(): begin" << std::endl;
  std::vector<std::shared_ptr<HObject>> snapshot = collectAliveObjects();
  std::cout << "flushQueues(): collected snapshot" << std::endl;
  std::vector<std::future<void>> futures;
  futures.reserve(snapshot.size());
  std::cout << "flushQueues(): reserved futures" << std::endl;
  for (auto &obj : snapshot) {
    auto p = std::make_shared<std::promise<void>>();
    futures.push_back(p->get_future());
    auto stopEvent = [this, p]() mutable { p->set_value(); };
    std::cout << "flushQueues(): adding stop event" << std::endl;
    obj->post(std::move(stopEvent)); // Add a stop event to the HObject
        std::cout << "flushQueues(): added stop event" << std::endl;

  }
  // Wait for all stop events to be executed
  std::cout << "flushQueues(): waiting for futures" << std::endl;

  for (auto &f : futures) {
    f.get();
  }
}

void EventLoop::Impl::unsubscribe() {
  std::vector<std::shared_ptr<HObject>> snapshot = collectAliveObjects();
  for (auto &obj : snapshot)
    obj->unsubscribe();
}

void EventLoop::Impl::subscribe() {
  std::vector<std::shared_ptr<HObject>> snapshot = collectAliveObjects();
  for (auto &obj : snapshot)
    obj->subscribe([this] { t_.notify(); });
}

} // namespace helios::core
