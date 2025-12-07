#pragma once

#include <fstream>

#include <core/h_loop.hpp>
#include <core/in_active_h_object.hpp>

namespace helios::logger {

/**
 * @class logger::FileSink
 *
 * @brief Takes a message and outputs it to a given file.
 *
 * @details
 * - Does not make any modifications to the message, not even adding a new line.
 */
class FileSink : public core::InActiveHObject {
public:
  /**
   * @brief Constructor.
   *
   * @param hBus Bus used for log messages.
   * @param filePath Path of the file used for logging.
   */
  FileSink(
      std::shared_ptr<core::HLoop> loop, std::shared_ptr<core::HBus> hBus,
      const std::string &filePath
  );

  /**
   * @brief Default virtual destructor.
   */
  ~FileSink() override = default;

private:
  /**
   * @brief File stream.
   */
  std::ofstream file_;
}; // class FileSink

} // namespace helios::logger
