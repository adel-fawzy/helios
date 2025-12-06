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

StandardOutputSink::StandardOutputSink(std::shared_ptr<core::HBus> hBus)
    : HObject(hBus) {
  listen<LogMessage>([this](auto logMessage) {
    auto e = [this, msg = logMessage->msg] { write(msg); };
    post(e);
  });
}

} // namespace helios::logger
