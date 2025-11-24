#include "core/h_object.hpp"

namespace helios::core {

HObject::HObject(std::shared_ptr<SignalBus> signalBus)
    : signalBus_(signalBus), id_(++nextId_) {}

HObject::~HObject() {
  if (signalBus_)
    signalBus_->unlisten(nextId_);
}

void HObject::post(const Event &e) { eventQueue_.post(e); }

void HObject::post(Event &&e) { eventQueue_.post(std::move(e)); }

bool HObject::tryPopAndExecute() { return eventQueue_.tryPopAndExecute(); }

bool HObject::hasEvents() { return !eventQueue_.empty(); }

} // namespace helios::core
