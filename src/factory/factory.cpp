#include "factory/factory.hpp"

#include "core/event_loop.hpp"
#include "core/event_queue.hpp"
#include "core/signal_bus.hpp"

namespace helios::factory {

std::shared_ptr<core::IEventQueue> createEventQueue() {
  return std::make_shared<core::EventQueue>();
}

std::shared_ptr<core::IEventLoop>
createEventLoop(std::shared_ptr<core::EventQueue> eventQueue) {
  return std::make_shared<core::EventLoop>(eventQueue);
}

std::shared_ptr<core::ISignalBus> createSignalBus() {
  return std::make_shared<core::SignalBus>();
}

} // namespace helios::factory
