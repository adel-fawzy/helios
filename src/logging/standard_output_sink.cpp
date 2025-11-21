#include "standard_output_sink.hpp"

#include <iostream>

namespace helios::logging {

StandardOutputSink::StandardOutputSink(
    std::shared_ptr<core::IEventQueue> eventQueue)
    : Module(eventQueue) {}

void StandardOutputSink::writeImpl(std::string msg) {
  add([msg = std::move(msg)] {
    std::cout << msg;
    std::cout.flush();
  });
}

} // namespace helios::logging
