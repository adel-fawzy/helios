#pragma once

#include "h_object.hpp"

namespace helios::core {

/**
 * @class core::InActiveHObject
 *
 * @brief Adds passive event execution to HObject.
 */
class InActiveHObject : public HObject {
public:
  /**
   * @brief Constructor.
   *
   * @param signalBus Optional shared pointer to the signal bus.
   */
  InActiveHObject(std::shared_ptr<SignalBus> signalBus);

  /**
   * @brief Default destructor.
   */
  ~InActiveHObject() override;

  /**
   * @brief Delete copy and move semantics.
   */
  InActiveHObject(const InActiveHObject &) = delete;
  InActiveHObject &operator=(const InActiveHObject &) = delete;
  InActiveHObject(InActiveHObject &&) = delete;
  InActiveHObject &operator=(InActiveHObject &&) = delete;

protected:
  void onEventPosted() override;

private:
  /**
   * @brief Subscriber's callback.
   */
  std::function<void()> cb_;

  /**
   * @brief Handles the first event in the queue and then returns.
   *
   * @return True if an event is handled, false otherwise.
   */
  bool tryPopAndExecute();

  /**
   * @brief Subscribes to be notified when an event is posted.
   */
  void subscribe(std::function<void()> cb);

  /**
   * @brief Unsubscribes.
   */
  void unsubscribe();

  /**
   * @brief EventLoop needs friend access to run the InActiveHObject.
   */
  friend class EventLoop;
}; // class InActiveHObject

}; // namespace helios::core
