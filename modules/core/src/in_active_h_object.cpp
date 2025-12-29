#include "core/in_active_h_object.hpp"

#include <future>

namespace helios::core {

InActiveHObject::InActiveHObject(std::shared_ptr<HLoop> loop,
                                 std::shared_ptr<HBus> hBus)
    : HObject(std::move(hBus)), loop_(loop) {}

InActiveHObject::~InActiveHObject() {
  // Post a stop event
  std::promise<void> finished;
  loop_->post([this, &finished] {
    finished.set_value(); // Indicate that the event has executed
  });
  finished.get_future().get(); // Wait for the stop event to be executed
}

void InActiveHObject::postImpl(Event e) { loop_->post(std::move(e)); }

} // namespace helios::core
