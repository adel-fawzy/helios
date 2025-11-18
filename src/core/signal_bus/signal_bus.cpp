#include "core/signal_bus/signal_bus.hpp"

namespace helios::core::signal_bus {

void SignalBus::subscribeImpl(ID id, std::type_index signalType,
                              RawCallback cb) {
  std::lock_guard<std::mutex> lock(_mtx);
  _signalsToSubscribers[signalType][id] = std::move(cb);
}

void SignalBus::publishImpl(std::type_index signalType, const void *signal) {
  SubscribersToCallbackMap subscribersToCallback;

  {
    std::lock_guard<std::mutex> lock(_mtx);
    auto it = _signalsToSubscribers.find(signalType);
    if (it == _signalsToSubscribers.end())
      // No subscribers for this signal type
      return;

    // Copy handlers so we unlock before executing callbacks
    subscribersToCallback = it->second;
  }

  for (auto &[id, wrapperCallback] : subscribersToCallback)
    wrapperCallback(signal);
}

void SignalBus::unsubscribe(ID id) {
  std::lock_guard<std::mutex> lock(_mtx);

  for (auto &[type, subscribersToCallback] : _signalsToSubscribers) {
    subscribersToCallback.erase(id);
  }
}

} // namespace helios::core::signal_bus
