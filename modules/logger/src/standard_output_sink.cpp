#include "standard_output_sink.hpp"

#include <iostream>

#include "log_message.hpp"

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
    std::shared_ptr<core::HLoop> loop, std::shared_ptr<core::HBus> hBus
)
    : InActiveHObject{loop, hBus} {
  LISTEN(LogMessage, { write(sig->msg); });
}

} // namespace helios::logger
