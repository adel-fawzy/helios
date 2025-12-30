#include "core/h_bus.hpp"

#include <mutex>
#include <unordered_map>

namespace helios::core {

class HBus::Impl {
public:
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
}; // class HBus::Impl

HBus::HBus() : impl_{std::make_unique<Impl>()} {}

HBus::~HBus() = default;

void HBus::listenImpl(
    std::type_index signalType, ID id,
    std::function<void(std::shared_ptr<const void>)> wrapperCallback
) {
  std::lock_guard<std::mutex> lock(impl_->mtx_);
  impl_->signalsMap_[signalType][id] = std::move(wrapperCallback);
}

void HBus::publishImpl(
    std::type_index signalType, std::shared_ptr<const void> signal
) {
  std::unordered_map<ID, std::function<void(std::shared_ptr<const void>)>>
      snapshot;

  {
    std::lock_guard<std::mutex> lock(impl_->mtx_);
    auto it = impl_->signalsMap_.find(signalType);
    if (it != impl_->signalsMap_.end())
      snapshot = it->second;
  }

  for (auto &[id, cb] : snapshot)
    cb(signal);
}

void HBus::unlisten(ID id) {
  std::lock_guard<std::mutex> lock(impl_->mtx_);

  for (auto &[type, listenersMap] : impl_->signalsMap_) {
    listenersMap.erase(id);
  }
}

} // namespace helios::core
