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
      return;

    // Copy handlers so we unlock before executing callbacks
    subscribersToCallback = it->second;
  }

  for (auto &[id, cb] : subscribersToCallback)
    cb(signal);
}

void SignalBus::unsubscribe(ID id) {
  std::lock_guard<std::mutex> lock(_mtx);

  for (auto &[type, subscribersToCallback] : _signalsToSubscribers) {
    subscribersToCallback.erase(id);
  }
}

} // namespace helios::core::signal_bus
