#pragma once

#include <memory>

#include <core/in_active_h_object.hpp>

namespace helios::logger {

/**
 * @class logger::StandardOutputSink
 *
 * @brief Takes a message and outputs it to the std::cout.
 *
 * @details
 * - Does not make any modifications to the message, not even adding a new line.
 */
class StandardOutputSink : public core::InActiveHObject {
public:
  /**
   * @brief Constructor.
   *
   * @param loop Event loop.
   * @param hBus Bus used for log messages.
   */
  StandardOutputSink(
      std::shared_ptr<core::HLoop> loop, core::HBus *hBus
  );

  /**
   * @brief Default virtual destructor.
   */
  ~StandardOutputSink() override = default;
}; // class StandardOutputSink

} // namespace helios::logger
