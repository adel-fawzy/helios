#pragma once

#include <core/h_object.hpp>
#include <memory>

namespace helios::logger {

/**
 * @class logger::StandardOutputSink
 *
 * @brief Takes a message and outputs it to the std::cout.
 *
 * @details
 * - Does not make any modifications to the message, not even adding a new line.
 */
class StandardOutputSink : public core::HObject {
public:
  /**
   * @brief Constructor.
   *
   * @param hBus Bus used for log messages.
   */
  StandardOutputSink(std::shared_ptr<core::HBus> hBus);

  /**
   * @brief Default virtual destructor.
   */
  ~StandardOutputSink() override = default;
}; // class StandardOutputSink

} // namespace helios::logger
