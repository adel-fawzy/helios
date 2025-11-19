#pragma once

#include <memory>

#include "core/event_loop/interface.hpp"
#include "core/event_queue/interface.hpp"
#include "core/signal_bus/interface.hpp"

namespace helios::factory {

/**
 * @brief Creates and returns a shared pointer to an EventQueue instance.
 *
 * @details
 * - The EventQueue is responsible for queuing and handling events.
 * - The EventQueue instance in itself does not create any threads.
 * - The EventQueue is thread-safe.
 * - In order to give the EventQueue its own thread of execution, create an
 * EventLoop.
 *
 * @return Shared pointer to the created EventQueue.
 */
std::shared_ptr<core::event_queue::Interface> createEventQueue();

/**
 * @brief Creates and returns a shared pointer to an EventLoop instance.
 *
 * - The EventLoop is responsible for running the event queue in a separate
 *   thread.
 * - The EventLoop repeatedly calls the handle() method of the provided
 *   EventQueue.
 *
 * @param eventQueue Shared pointer to the EventQueue to be used by the
 *                   EventLoop.
 * @return Shared pointer to the created EventLoop.
 */
std::shared_ptr<core::event_loop::Interface>
createEventLoop(std::shared_ptr<core::event_queue::Interface> eventQueue);

/**
 * @brief Creates and returns a shared pointer to a SignalBus instance.
 *
 * - Used with the class 'Module'.
 * - The SignalBus is responsible for managing signal subscriptions and
 *   publications between different modules.
 * - Modules can subscribe to specific signal types and publish signals to
 *   notify subscribers.
 *
 * @return Shared pointer to the created SignalBus.
 */
std::shared_ptr<core::signal_bus::Interface> createSignalBus();

} // namespace helios::factory
