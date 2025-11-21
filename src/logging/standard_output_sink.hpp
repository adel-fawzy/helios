#pragma once

#include <memory>

#include "core/ievent_queue.hpp"
#include "core/module.hpp"
#include "logging/isink.hpp"

namespace helios::logging {

/**
 * @class StandardOutputSink
 *
 * @brief Takes a message and outputs it to the std::cout.
 *
 * @details
 * - Does not make any modifications to the message, not even adding a new line.
 */
class StandardOutputSink : public ISink, public core::Module {
public:
  /**
   * @brief Constructor.
   */
  StandardOutputSink(std::shared_ptr<core::IEventQueue> eventQueue);

  /**
   * @brief Default destructor.
   */
  virtual ~StandardOutputSink() override = default;

  void writeImpl(std::string msg) override;
};

} // namespace helios::logging
