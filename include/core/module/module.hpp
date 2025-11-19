#pragma once

#include <memory>

#include "core/event_queue/interface.hpp"
#include "core/signal_bus/interface.hpp"

namespace helios::core::module {

/**
 * @class module::Module
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
  Module(std::shared_ptr<event_queue::Interface> eventQueue,
         std::shared_ptr<signal_bus::Interface> signalBus = nullptr)
      : _eventQueue(eventQueue), _signalBus(signalBus), _id(++_nextId) {}

  /**
   * @brief Virtual destructor.
   */
  virtual ~Module() {
    if (_signalBus)
      _signalBus->unsubscribe(_id);
  }

protected:
  /**
   * @brief Adds an event to the module's event queue.
   *
   * @param event The event to be added.
   */
  template <typename E> void add(E &&event) {
    _eventQueue->push(std::forward<E>(event));
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
    if (_signalBus)
      _signalBus->subscribe<SignalT>(_id, std::forward<Callback>(cb));
  }

  /**
   * @brief Publishes a signal to the signal bus.
   *
   * @tparam SignalT The signal type to publish.
   * @param s The signal instance to publish.
   */
  template <typename SignalT> void publishSignal(const SignalT &s) {
    if (_signalBus)
      _signalBus->publish<SignalT>(s);
  }

private:
  /**
   * @brief ID of the next module to be created.
   */
  static inline ID _nextId{0};

  /**
   * @brief Event queue of the module.
   */
  std::shared_ptr<event_queue::Interface> _eventQueue;

  /**
   * @brief Shared pointer to the signal bus.
   */
  std::shared_ptr<signal_bus::Interface> _signalBus;

  /**
   * @brief Unique module ID.
   */
  const ID _id;
};

} // namespace helios::core::module
