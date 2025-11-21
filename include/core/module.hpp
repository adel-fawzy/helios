#pragma once

#include <memory>

#include "core/ievent_queue.hpp"
#include "core/isignal_bus.hpp"

namespace helios::core {

/**
 * @class core::Module
 *
 * @brief Concrete implementation of a module.
 *
 * @details
 * - When 'Module::add' function is called, the class shall call
 *   'event_queue::Interface::push' to add an event to the event queue.
 * - When 'Module::subscribeSignal' function is called, the class shall call
 *   'signal_bus::Interface::subscribe' to subscribe to the signal.
 * - When 'Module::publishSignal' function is called, the class shall call
 *   'signal_bus::Interface::publish' to publish the signal.
 * - When 'Module::~Module' is called, the class shall call
 *   'signal_bus::Interface::unsubscribe' to unsubscribe from the signals that
 *   it subscribed to.
 * - The signal bus is optional. A module can live without a signal bus but it
 *   cannot live without an event queue.
 */
class Module {
public:
  /**
   * @brief Constructor.
   */
  Module(std::shared_ptr<IEventQueue> eventQueue,
         std::shared_ptr<ISignalBus> signalBus = nullptr)
      : eventQueue_(eventQueue), signalBus_(signalBus), id_(++nextId_) {}

  /**
   * @brief Virtual destructor.
   */
  virtual ~Module() {
    if (signalBus_)
      signalBus_->unsubscribe(nextId_);
  }

protected:
  /**
   * @brief Adds an event to the module's event queue.
   *
   * @param event The event to be added.
   */
  template <typename E> void add(E &&event) {
    eventQueue_->push(std::forward<E>(event));
  }

  /**
   * @brief Subscribes to a signal type.
   *
   * @tparam SignalT The signal type to subscribe to.
   * @tparam Callback The type of the callback function.
   * @param cb The callback function to invoke when the signal is published.
   */
  template <typename SignalT, typename Callback>
  void subscribeSignal(Callback &&cb) {
    if (signalBus_)
      signalBus_->subscribe<SignalT>(id_, std::forward<Callback>(cb));
  }

  /**
   * @brief Publishes a signal to the signal bus.
   *
   * @tparam SignalT The signal type to publish.
   * @param s The signal instance to publish.
   */
  template <typename SignalT> void publishSignal(const SignalT &s) {
    if (signalBus_)
      signalBus_->publish<SignalT>(s);
  }

private:
  /**
   * @brief ID of the next module to be created.
   */
  static inline ID nextId_{0};

  /**
   * @brief Event queue of the module.
   */
  std::shared_ptr<IEventQueue> eventQueue_;

  /**
   * @brief Shared pointer to the signal bus.
   */
  std::shared_ptr<ISignalBus> signalBus_;

  /**
   * @brief Unique module ID.
   */
  const ID id_;
}; // class Module

} // namespace helios::core
