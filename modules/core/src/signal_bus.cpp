#include "core/signal_bus.hpp"

#include <mutex>
#include <unordered_map>

namespace helios::core {

class SignalBus::Impl {
public:
  void listen(std::type_index signalType, ID id,
              std::function<void(std::shared_ptr<const void>)> wrapperCallback);
  void publish(std::type_index signalType, std::shared_ptr<const void> signal);
  void unlisten(ID id);

private:
  /**
   * @brief Type alias for the callback of the listeners.
   */
  using Callback = std::function<void(std::shared_ptr<const void>)>;

  /**
   * @brief Type alias for listeners map.
   */
  using ListenersMap = std::unordered_map<ID, Callback>;

  /**
   * @brief Type alias for the signals map.
   */
  using SignalsMap = std::unordered_map<std::type_index, ListenersMap>;

  /**
   * @brief Holds the listeners for each signal.
   */
  SignalsMap signalsMap_;

  /**
   * @brief Mutex to protect the class.
   */
  std::mutex mtx_;
}; // class SignalBus::Impl

SignalBus::SignalBus() = default;

SignalBus::~SignalBus() = default;

void SignalBus::listenImpl(
    std::type_index signalType, ID id,
    std::function<void(std::shared_ptr<const void>)> wrapperCallback) {
  impl_->listen(typeid(signalType), id, std::move(wrapperCallback));
}

void SignalBus::publishImpl(std::type_index signalType,
                            std::shared_ptr<const void> signal) {
  impl_->publish(typeid(signalType), std::move(signal));
}

void SignalBus::unlisten(ID id) { impl_->unlisten(std::move(id)); }

void SignalBus::Impl::listen(
    std::type_index signalType, ID id,
    std::function<void(std::shared_ptr<const void>)> wrapperCallback) {
  std::lock_guard<std::mutex> lock(mtx_);
  signalsMap_[signalType][id] = std::move(wrapperCallback);
}

void SignalBus::Impl::publish(std::type_index signalType,
                              std::shared_ptr<const void> signal) {
  std::unordered_map<ID, std::function<void(std::shared_ptr<const void>)>>
      snapshot;

  {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = signalsMap_.find(signalType);
    if (it != signalsMap_.end())
      snapshot = it->second;
  }

  for (auto &cb : snapshot)
    cb.second(signal);
}

void SignalBus::Impl::unlisten(ID id) {
  std::lock_guard<std::mutex> lock(mtx_);

  for (auto &[type, listenersMap] : signalsMap_) {
    listenersMap.erase(id);
  }
}

} // namespace helios::core
