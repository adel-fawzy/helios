#pragma once

#include <functional>
#include <typeindex>

#include "core/types/id.hpp"

namespace helios::core::signal_bus {

/**
 * @class EventBus::Interface
 *
 * @brief Event bus that could be used for communication between event-driven
 *        classes.
 *
 * - Event-driven classes can use this bus to communicate with each other by
 *   subscribing to signals and publishing signals on the bus.
 * - Different clients are identified by their unique IDs.
 * - All public functions are thread-safe.
 */
class Interface {
public:
  /**
   * @brief Default constructor.
   */
  Interface() = default;

  /**
   * @brief Default virtual destructor.
   */
  virtual ~Interface() = default;

  /**
   * @brief Delete copy semantics.
   */
  Interface(const Interface &) = delete;
  Interface &operator=(const Interface &) = delete;

  /**
   * @brief Default move semantics.
   */
  Interface(Interface &&) = default;
  Interface &operator=(Interface &&) = default;

  /**
   * @brief Type alias for a raw callback function.
   */
  using RawCallback = std::function<void(const void *)>;

  /**
   * @brief Subscribes to a signal type.
   *
   * @tparam Signal   Signal that the subscriber wants to listen to.
   * @tparam Callback Callable type.
   * @param id        Subscribers' module ID.
   * @param cb        Callable invoked when the signal is published.
   *
   * - If the same ID subscribes multiple times to the same signal type,
   *   the last subscription will overwrite previous ones.
   *
   * @note The callback passed MUST NOT BLOCK, as it will be called
   *       synchronously. The callback must only post the work to be done to an
   *       event loop (another thread).
   */
  template <typename Signal, typename Callback>
  void subscribe(ID id, Callback &&cb) {
    RawCallback wrapper = [cb = std::move(cb)](const void *signal) {
      // Cast and dereference the signal being published
      cb(*static_cast<const Signal *>(signal)); // Call subscriber's callback
    };
    subscribeImpl(id, typeid(Signal), std::move(wrapper));
  }

  /**
   * @brief Publishes a signal to its subscribers.
   *
   * Delivers the provided `signal` to all subscribers registered for this
   * signal type, if any.
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
