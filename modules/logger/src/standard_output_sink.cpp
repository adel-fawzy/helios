#include "standard_output_sink.hpp"
#include "log_message.hpp"

#include <iostream>

namespace {

/**
 * @brief Writes a string to the standard output.
 *
 * @param s String to be written.
 */
void write(const std::string &s) {
  std::cout << s;
  std::cout.flush();
}

} // namespace

namespace helios::logger {

StandardOutputSink::StandardOutputSink(
    std::shared_ptr<core::IEventQueue> eventQueue,
    std::shared_ptr<core::SignalBus> signalBus)
    : Module(eventQueue, signalBus) {
  listen<LogMessage>([this](auto logMessage) {
    auto e = [this, msg = logMessage->msg] { write(msg); };
    add(e);
  });
}

} // namespace helios::logger
