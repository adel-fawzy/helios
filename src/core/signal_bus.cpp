#include "signal_bus.hpp"

namespace helios::core {

void SignalBus::subscribeImpl(ID id, std::type_index signalType,
                              RawCallback cb) {
  std::lock_guard<std::mutex> lock(mtx_);
  signalsToSubscribers_[signalType][id] = std::move(cb);
}

void SignalBus::publishImpl(std::type_index signalType, const void *signal) {
  SubscribersToCallbackMap subscribersToCallback;

  {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = signalsToSubscribers_.find(signalType);
    if (it == signalsToSubscribers_.end())
      // No subscribers for this signal type
      return;

    // Copy handlers so we unlock before executing callbacks
    subscribersToCallback = it->second;
  }

  for (auto &[id, wrapperCallback] : subscribersToCallback)
    wrapperCallback(signal);
}

void SignalBus::unsubscribe(ID id) {
  std::lock_guard<std::mutex> lock(mtx_);

  for (auto &[type, subscribersToCallback] : signalsToSubscribers_) {
    subscribersToCallback.erase(id);
  }
}

} // namespace helios::core
