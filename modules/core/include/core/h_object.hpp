#pragma once

#include <memory>

#include "event_queue.hpp"
#include "signal_bus.hpp"

namespace helios::core {

/**
 * @class core::HObject
 *
 * @brief Concrete implementation of a HObject.
 *
 * @details
 * - When 'HObject::add' function is called, the class shall call
 *   'event_queue::Interface::push' to add an event to the event queue.
 * - When 'HObject::subscribeSignal' function is called, the class shall call
 *   'signal_bus::Interface::subscribe' to subscribe to the signal.
 * - When 'HObject::publishSignal' function is called, the class shall call
 *   'signal_bus::Interface::publish' to publish the signal.
 * - When 'HObject::~HObject' is called, the class shall call
 *   'signal_bus::Interface::unsubscribe' to unsubscribe from the signals that
 *   it subscribed to.
 * - The signal bus is optional. A HObject can live without a signal bus but it
 *   cannot live without an event queue.
 */
class HObject {
public:
  /**
   * @brief Constructor.
   */
  HObject(std::shared_ptr<SignalBus> signalBus = nullptr)
      : signalBus_(signalBus), id_(++nextId_) {}

  /**
   * @brief Virtual destructor.
   */
  virtual ~HObject() {
    if (signalBus_)
      signalBus_->unlisten(nextId_);
  }

  bool tryPopAndExecute() { return eventQueue_.tryPopAndExecute(); }

  bool hasEvents() { return !eventQueue_.empty(); }

protected:
  /**
   * @brief Adds an event to the HObject's event queue.
   *
   * @param event The event to be added.
   */
  // template <typename E> void add(E &&event) {
  //   eventQueue_->push(std::forward<E>(event));
  // }

  /**
   * @brief Listens to a signal type.
   *
   * @tparam SignalT The signal type to listen to.
   * @tparam Callback The type of the callback function.
   * @param cb The callback function to invoke when the signal is published.
   */
  template <typename SignalT, typename Callback> void listen(Callback &&cb) {
    if (signalBus_)
      signalBus_->listen<SignalT>(id_, std::forward<Callback>(cb));
  }

  /**
   * @brief Publishes a signal to the signal bus.
   *
   * @tparam SignalT The signal type to publish.
   * @param s The signal instance to publish.
   */
  template <typename SignalT> void publish(SignalT s) {
    if (signalBus_)
      signalBus_->publish<SignalT>(s);
  }

private:
  /**
   * @brief ID of the next HObject to be created.
   */
  static inline ID nextId_{0};

  /**
   * @brief Event queue of the HObject.
   */
  EventQueue eventQueue_;

  /**
   * @brief Shared pointer to the signal bus.
   */
  std::shared_ptr<SignalBus> signalBus_;

  /**
   * @brief Unique HObject ID.
   */
  const ID id_;
}; // class HObject

} // namespace helios::core
