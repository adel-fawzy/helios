#pragma once

#include <condition_variable>
#include <deque>
#include <exception>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>

#include "event.hpp"
#include "signal_bus.hpp"

namespace helios::core {

/**
 * @class core::HObject
 *
 * @brief Abstract base class that provides its derived classes asynchronous
 *        event handling and access to a signal bus.
 *
 * @details
 * - Provides the derived classes with:
 *   - Asynchronous event handling.
 *   - Communication with the signal bus using a unique ID.
 * - The signal bus is optional. An HObject can live without it.
 * - Clients are expected to catch exceptions in the event in case any are
 *   thrown. However, this class has a safety mechanism such that it catches any
 *   exceptions not handled inside the event.
 *
 * @note
 * - All public functions are asynchronous except the post() function which must
 *   be synchronous since it is communicating with the event queue.
 * - All public functions are thread-safe.
 * - shutdown() must be called before ~HObject(). Note that shutdown() blocks
 *   until the stop event is executed. This means that the derived class must
 *   somehow ensure that a thread is running the queue while shutdown() is
 *   called.
 */
class HObject {
public:
  /**
   * @brief Constructor.
   *
   * @param signalBus Optional shared pointer to the signal bus.
   */
  HObject(std::shared_ptr<SignalBus> bus = nullptr)
      : bus_(bus), id_(++nextId_) {
    std::cout << "HObject: Created" << std::endl;
  }

  /**
   * @brief Virtual destructor.
   */
  virtual ~HObject() = default;

  /**
   * @brief Delete copy and move semantics.
   */
  HObject(const HObject &) = delete;
  HObject &operator=(const HObject &) = delete;
  HObject(HObject &&) = delete;
  HObject &operator=(HObject &&) = delete;

protected:
  /**
   * @brief Adds an event to the queue.
   *
   * @details
   * - Any events posted during the destruction process will be silently
   *   discarded.
   *
   * @tparam EventT Type of Event to be added.
   * @param e Event to be added.
   */
  template <typename EventT> void post(EventT &&e) {
    if (stopReceivingEvents_)
      return;
    push(std::forward<EventT>(e));
    onEventPosted();
  }

  /**
   * @brief Optional template method for derived classes.
   */
  virtual void onEventPosted() = 0;

  /**
   * @brief Pops the event next in line.
   *
   * @return Popped event. nullptr if the queue is empty.
   */
  // std::optional<Event> pop() {
  //   std::cout << "HObject::pop: Acquiring mutex" << std::endl;
  //   std::lock_guard<std::mutex> lock(mtx_);
  //   std::cout << "HObject::pop: Acquired mutex" << std::endl;
  //   if (q_.empty())
  //     return std::nullopt;
  //   std::optional<Event> e = std::move(q_.front());
  //   q_.pop();
  //   std::cout << "HObject::pop: Popped event" << std::endl;
  //   return e;
  // }

  /**
   * @brief Returns true if the queue has events, false otherwise.
   */
  bool hasEvents() {
    std::lock_guard<std::mutex> lock(mtx_);
    return !q_.empty();
  }

  /**
   * @brief Handles an event.
   */
  void handle(const Event &event) {
    try {
      std::cout << "HObject::handle: Handling event" << std::endl;
      event(); // Handle event
      std::cout << "HObject::handle: Handled event" << std::endl;
    } catch (const std::exception &exception) {
      // TODO: Log (Verbose) that an exception is catched (use e.what())
    } catch (...) {
      // TODO: Log (Verbose) that an exception is catched
    }
  }

  /**
   * @brief Listens to a signal type.
   *
   * @tparam SignalT The signal type to listen to.
   * @tparam CallbackT The type of the callback function.
   * @param cb The callback function to invoke when the signal is published.
   *
   * @note
   * - Will silently discard the request in case it is called during object
   *   destruction.
   */
  template <typename SignalT, typename CallbackT> void listen(CallbackT &&cb) {
    post([this, cb = std::forward<CallbackT>(cb)] {
      if (bus_)
        bus_->listen<SignalT>(id_, std::move(cb));
    });
  }

  /**
   * @brief Publishes a signal to the signal bus.
   *
   * @tparam SignalT The signal type to publish.
   * @param s The signal to publish.
   *
   * @note
   * - Will silently discard the request in case it is called during object
   *   destruction.
   */
  template <typename SignalT> void publish(SignalT &&s) {
    post([this, s = std::forward<SignalT>(s)] {
      if (bus_)
        bus_->publish<SignalT>(std::move(s));
    });
  }

  /**
   * @brief Stops receiving new events, then adds a stop event and waits for it
   *        to be executed.
   *
   * @details
   * - Adds a stop event to the queue and waits until it is executed.
   *
   * @note
   * - This function will block if there is no thread running the queue.
   */
  void shutdown() {
    std::cout << "HObject::shutdown: Called" << std::endl;
    stopReceivingEvents_ = true;

    auto pr = std::make_shared<std::promise<void>>();
    auto fut = pr->get_future();

    // Push the stop event
    push([this, pr]() mutable {
      if (bus_)
        bus_->unlisten(id_);
      pr->set_value();
    });
    std::cout << "HObject::shutdown: Waiting for the stop event to execute"
              << std::endl;
    fut.get(); // Wait for the stop event to be executed
    std::cout << "HObject::shutdown: Stop event executed" << std::endl;
  }

  /**
   * @brief Event queue of the HObject.
   */
  std::deque<Event> q_;

  /**
   * @brief Used to let the pop() function sleep.
   */
  std::condition_variable cv_;

  /**
   * @brief Unique HObject ID.
   */
  const ID id_;

  /**
   * @brief Protects this class.
   */
  std::mutex mtx_;

private:
  /**
   * @brief ID of the next HObject to be created.
   */
  static inline std::atomic<ID> nextId_{0};

  /**
   * @brief Shared pointer to the signal bus.
   */
  std::shared_ptr<SignalBus> bus_;

  /**
   * @brief Set to true to stop receiving events.
   */
  std::atomic<bool> stopReceivingEvents_{false};

  /**
   * @brief Pushes an event to the queue.
   *
   * @tparam Type of event.
   * @param e Event to be pushed.
   */
  template <typename EventT> void push(EventT &&e) {
    std::cout << "HObject: Pushing" << std::endl;
    {
      std::cout << "HObject::push: Acquiring mutex" << std::endl;
      std::lock_guard<std::mutex> lock(mtx_);
      std::cout << "HObject::push: Acquired mutex" << std::endl;
      q_.push_back(std::forward<EventT>(e));
      std::cout << "HObject::push: Pushed an event" << std::endl;
    }
    std::cout << "HObject::push: Notifying thread" << std::endl;
    cv_.notify_one();
    std::cout << "HObject::push: Notified thread" << std::endl;
  }
}; // class HObject

} // namespace helios::core
