#include "core/in_active_h_object.hpp"

namespace helios::core {

InActiveHObject::InActiveHObject(std::shared_ptr<SignalBus> signalBus)
    : HObject(std::move(signalBus)) {}

InActiveHObject::~InActiveHObject() { shutdown(); }

bool InActiveHObject::tryPopAndExecute() {
  // std::optional<Event> e = HObject::pop();
  // if (!e)
  //   return false;
  // handle(*e);
  // return true;
}

void InActiveHObject::onEventPosted() {
  if (cb_)
    cb_(); // Notify subscriber that an event is posted
}

void InActiveHObject::subscribe(std::function<void()> cb) {
  post([this, cb = std::move(cb)] { cb_ = cb; });
}

void InActiveHObject::unsubscribe() {
  post([this] { cb_ = nullptr; });
}

} // namespace helios::core
