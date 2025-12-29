#pragma once

#include <functional>
#include <memory>
#include <typeindex>

#include "id.hpp"

namespace helios::core {

/**
 * @class core::HBus
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
class HBus {
public:
  /**
   * @brief Constructor.
   */
  HBus();

  /**
   * @brief Destructor.
   */
  ~HBus();

  /**
   * @brief Delete copy and move semantics.
   */
  HBus(const HBus &) = delete;
  HBus &operator=(const HBus &) = delete;
  HBus(HBus &&) = delete;
  HBus &operator=(HBus &&) = delete;

  /**
   * @brief Listens to a signal.
   *
   * @tparam SignalT Type of the signal to listen to.
   * @param id ID of the HObject that wants to listener.
   * @param listenerCallback Callback that will be called when the signal is
   *        published on the bus.
   */
  template <typename SignalT>
  void listen(
      ID id,
      std::function<void(std::shared_ptr<const SignalT>)> listenerCallback
  ) {
    // Wrap user callback
    auto wrapper = [cb = std::move(listenerCallback)](
                       std::shared_ptr<const void> s
                   ) { cb(std::static_pointer_cast<const SignalT>(s)); };

    listenImpl(typeid(SignalT), id, std::move(wrapper));
  }

  /**
   * @brief Publishes a signal on the bus.
   *
   * @tparam SignalT Type of the published signal.
   * @param s The published signal.
   */
  template <typename SignalT>
  void publish(SignalT &&s) {
    using T = std::remove_cv_t<std::remove_reference_t<SignalT>>;
    publishImpl(typeid(T), std::make_shared<T>(std::forward<SignalT>(s)));
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

  void listenImpl(
      std::type_index signalType, ID id,
      std::function<void(std::shared_ptr<const void>)> wrapperCallback
  );

  void
  publishImpl(std::type_index signalType, std::shared_ptr<const void> signal);
}; // class HBus

} // namespace helios::core
