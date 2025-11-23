#pragma once

#include <fstream>

#include "core/module.hpp"

namespace helios::logger {

/**
 * @class logger::FileSink
 *
 * @brief Takes a message and outputs it to a given file.
 *
 * @details
 * - Does not make any modifications to the message, not even adding a new line.
 */
class FileSink : public core::Module {
public:
  /**
   * @brief Constructor.
   *
   * @param eventQueue Event queue used by this class.
   * @param signalBus Bus used for listening to the log messages.
   * @param filePath Path of the file used for logging.
   */
  FileSink(std::shared_ptr<core::IEventQueue> eventQueue,
           std::shared_ptr<core::SignalBus> signalBus,
           const std::string &filePath);

  /**
   * @brief Default destructor.
   */
  virtual ~FileSink() = default;

private:
  /**
   * @brief File stream.
   */
  std::ofstream file_;
};

} // namespace helios::logger
