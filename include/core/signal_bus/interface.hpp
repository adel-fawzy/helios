#pragma once

#include <functional>
#include <typeindex>

#include "core/types/id.hpp"

namespace helios::core::signal_bus {
/**
 * @class EventBus::Interface
 *
 * @brief Thread-safe event bus.
 *
 * Event-driven classes can use this bus to communicate with each other by
 * subscribing to signals and publishing signals on the bus.
 *
 * All public functions are thread-safe.
 */
class Interface {
public:
  /**
   * @brief Type alias for a raw callback function.
   */
  using RawCallback = std::function<void(const void *)>;

  /**
   * @brief Virtual destructor.
   */
  virtual ~Interface() = default;

  /**
   * @brief Subscribe to a signal type.
   *
   * @tparam Signal   The signal type to subscribe to.
   * @param id        Module ID.
   * @param cb        Callable invoked when the signal is published.
   *
   * - If the same ID subscribes multiple times to the same signal type,
   *   the last subscription will overwrite previous ones.
   * - The callback is moved into the bus; do not use it after calling this
   *   method.
   */
  template <typename Signal>
  void subscribe(ID id, std::function<void(const Signal &)> cb) {
    RawCallback wrapper = [cb = std::move(cb)](const void *raw) {
      cb(*static_cast<const Signal *>(raw));
    };
    subscribeImpl(id, typeid(Signal), std::move(wrapper));
  }

  /**
   * @brief Publishes a signal to its subscribers.
   *
   * Delivers the provided `signal` to all subscribers registered for this
   * signal type.
   *
   * @tparam Signal The signal type being published.
   * @param signal  The signal instance to deliver.
   *
   * @note If there are no subscribers for this signal type, this method
   *       performs no action.
   */
  template <typename Signal> void publish(const Signal &signal) {
    publishImpl(typeid(Signal), &signal);
  }

  /**
   * @brief Unsubscribe the given id from all signals.
   *
   * Removes all subscriptions associated with `id`.
   *
   * @param id Subscriber identifier previously used in subscribe().
   */
  virtual void unsubscribe(ID id) = 0;

protected:
  virtual void subscribeImpl(ID id, std::type_index signalType,
                             RawCallback cb) = 0;

  virtual void publishImpl(std::type_index signalType, const void *signal) = 0;
}; // class Interface
} // namespace helios::core::signal_bus
