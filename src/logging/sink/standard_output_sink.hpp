#pragma once

#include <memory>

#include "core/module/module.hpp"
#include "core/event_queue/interface.hpp"
#include "logging/sink/interface.hpp"

namespace helios::logging::sink {

class StandardOutputSink : public Interface, public core::module::Module {
public:
  /**
   * @brief Default constructor
   */
  StandardOutputSink(std::shared_ptr<core::event_queue::Interface> eventQueue);

  /**
   * @brief Default destructor
   */
  virtual ~StandardOutputSink() override = default;

  void writeImpl(std::string msg) override;
};

} // namespace helios::logging::sink
