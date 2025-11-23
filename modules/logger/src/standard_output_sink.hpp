#pragma once

#include <memory>

#include "core/ievent_queue.hpp"
#include "core/module.hpp"

namespace helios::logger {

/**
 * @class logger::StandardOutputSink
 *
 * @brief Takes a message and outputs it to the std::cout.
 *
 * @details
 * - Does not make any modifications to the message, not even adding a new line.
 */
class StandardOutputSink : public core::Module {
public:
  /**
   * @brief Constructor.
   *
   * @param eventQueue Event queue used by this class.
   * @param signalBus Bus used for listening to the log messages.
   */
  StandardOutputSink(std::shared_ptr<core::IEventQueue> eventQueue,
                     std::shared_ptr<core::SignalBus> signalBus);

  /**
   * @brief Default destructor.
   */
  virtual ~StandardOutputSink() override = default;
};

} // namespace helios::logger
