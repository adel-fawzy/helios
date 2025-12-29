#pragma once

#include "active_h_object.hpp"

namespace helios::core {

class HLoop : public ActiveHObject {
public:
  /**
   * @brief Posts an event to the queue.
   *
   * @tparam EventT Type of event to be posted.
   * @param e Event to be posted.
   */
  template <typename EventT> void post(EventT &&e) {
    ActiveHObject::post(std::forward<EventT>(e));
  }
}; // class HLoop

} // namespace helios::core
