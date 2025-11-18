#include "factory/factory.hpp"

#include "core/event_loop/event_loop.hpp"
#include "core/event_queue/event_queue.hpp"
#include "core/signal_bus/signal_bus.hpp"

namespace helios::factory {

std::shared_ptr<core::event_queue::Interface> createEventQueue() {
  return std::make_shared<core::event_queue::EventQueue>();
}

std::shared_ptr<core::event_loop::Interface>
createEventLoop(std::shared_ptr<core::event_queue::Interface> eventQueue) {
  return std::make_shared<core::event_loop::EventLoop>(eventQueue);
}

std::shared_ptr<core::signal_bus::Interface> createSignalBus() {
  return std::make_shared<core::signal_bus::SignalBus>();
}

} // namespace helios::factory
