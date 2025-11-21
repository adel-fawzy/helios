#pragma once

#include <memory>

#include "core/ievent_queue.hpp"
#include "core/module.hpp"
#include "logging/ifile.hpp"
#include "logging/isink.hpp"

namespace helios::logging {

/**
 * @class FileSink
 *
 * @brief Takes a message and outputs it to a given file.
 *
 * - Does not make any modifications to the message, not even adding a new line.
 */
class FileSink : public ISink, public core::Module {
public:
  /**
   * @brief Constructor.
   */
  FileSink(std::unique_ptr<IFile> file,
           std::shared_ptr<core::IEventQueue> eventQueue);

  /**
   * @brief Default destructor.
   */
  virtual ~FileSink() override = default;

protected:
  void writeImpl(std::string msg) override;

private:
  /**
   * @brief File to append.
   */
  std::unique_ptr<IFile> file_;
};

} // namespace helios::logging::sink
