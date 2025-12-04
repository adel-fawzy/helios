#pragma once

#include <functional>
#include <memory>
#include <typeindex>

#include "id.hpp"

namespace helios::core {

/**
 * @class core::SignalBus
 *
 * @brief Used for communication between event-driven objects.
 *
 * @details
 * - Event-driven classes can use this bus to communicate with each other by
 *   listening to signals and publishing signals on the bus.
 * - Different objects are identified by their unique IDs.
 *
 * @note
 * - All public functions are thread-safe.
 * - All public functions are synchronous.
 */
class SignalBus {
public:
  /**
   * @brief Default constructor.
   */
  SignalBus();

  /**
   * @brief Destructor.
   */
  ~SignalBus();

  /**
   * @brief Delete copy and move semantics.
   */
  SignalBus(const SignalBus &) = delete;
  SignalBus &operator=(const SignalBus &) = delete;
  SignalBus(SignalBus &&) = delete;
  SignalBus &operator=(SignalBus &&) = delete;

  /**
   * @brief Listens to a signal.
   *
   * @tparam SignalType Type of the signal to listen to.
   * @param id ID of the HObject that wants to listener.
   * @param listenerCallback Callback that will be called when the signal is
   *        published on the bus.
   */
  template <typename SignalType>
  void listen(
      ID id,
      std::function<void(std::shared_ptr<const SignalType>)> listenerCallback) {
    // Wrap user callback
    auto wrapper =
        [cb = std::move(listenerCallback)](std::shared_ptr<const void> s) {
          cb(std::static_pointer_cast<const SignalType>(s));
        };

    listenImpl(typeid(SignalType), id, std::move(wrapper));
  }

  /**
   * @brief Publishes a signal on the bus.
   *
   * @tparam SignalType Type of the published signal.
   * @param s The published signal.
   */
  template <typename SignalType> void publish(SignalType &&s) {
    using T = std::remove_cv_t<std::remove_reference_t<SignalType>>;
    publishImpl(typeid(T), std::make_shared<T>(std::forward<SignalType>(s)));
  }

  /**
   * @brief Stops listening to all signals for a given ID.
   *
   * @param id Listener identifier previously used in listen().
   */
  void unlisten(ID id);

private:
  /**
   * @brief Forward declaration for the implementation class.
   */
  class Impl;

  /**
   * @brief Unique pointer to the implementation class.
   */
  std::unique_ptr<Impl> impl_;

  void
  listenImpl(std::type_index signalType, ID id,
             std::function<void(std::shared_ptr<const void>)> wrapperCallback);

  void publishImpl(std::type_index signalType,
                   std::shared_ptr<const void> signal);
}; // class SignalBus

} // namespace helios::core
