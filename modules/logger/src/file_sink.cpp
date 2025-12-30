#include "file_sink.hpp"

#include "log_message.hpp"

namespace {

/**
 * @brief Writes a string to a file.
 *
 * @param s String to be written.
 * @param file File to be written.
 */
void write(const std::string &s, std::ofstream &file) {
  if (file.is_open()) {
    file << s;
    file.flush();
  }
}

} // namespace

namespace helios::logger {

FileSink::FileSink(
    std::shared_ptr<core::HLoop> loop, core::HBus *hBus,
    const std::string &filePath
)
    : InActiveHObject{loop, hBus}, file_{filePath} {
  LISTEN(LogMessage, { write(sig->msg, file_); });
}

} // namespace helios::logger
