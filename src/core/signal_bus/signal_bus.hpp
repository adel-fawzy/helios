#pragma once

#include <functional>
#include <mutex>
#include <typeindex>
#include <unordered_map>

#include "core/signal_bus/interface.hpp"
#include "core/types/id.hpp"

namespace helios::core::signal_bus {

/**
 * @class signal_bus::SignalBus
 *
 * @brief Concrete implementation of the signal bus interface.
 */
class SignalBus : public Interface {
public:
  /**
   * @brief Default constructor.
   */
  SignalBus() = default;

  /**
   * @brief Default destructor.
   */
  ~SignalBus() = default;

  /**
   * @brief Delete copy and move semantics.
   */
  SignalBus(const SignalBus &) = delete;
  SignalBus &operator=(const SignalBus &) = delete;
  SignalBus(SignalBus &&) = delete;
  SignalBus &operator=(SignalBus &&) = delete;

  void unsubscribe(ID id) override;

protected:
  void subscribeImpl(ID id, std::type_index signalType,
                     RawCallback cb) override;

  void publishImpl(std::type_index signalType, const void *signal) override;

private:
  /**
   * @brief Type alias for subscribers map.
   */
  using SubscribersToCallbackMap = std::unordered_map<ID, RawCallback>;

  /**
   * @brief Type alias for the signals map.
   */
  using SignalsToSubscribersMap =
      std::unordered_map<std::type_index, SubscribersToCallbackMap>;

  /**
   * @brief Holds the subscribers for each signal.
   */
  SignalsToSubscribersMap _signalsToSubscribers;

  /**
   * @brief Mutex to protect the class.
   */
  std::mutex _mtx;
};

} // namespace helios::core::signal_bus
