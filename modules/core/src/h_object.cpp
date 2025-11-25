#include "core/h_object.hpp"

namespace helios::core {

HObject::HObject(std::shared_ptr<SignalBus> signalBus)
    : signalBus_(signalBus), id_(++nextId_) {}

HObject::~HObject() {
  if (signalBus_)
    signalBus_->unlisten(nextId_);
}

void HObject::post(const Event &e) {
  eventQueue_.post(e);
  notify();
}

void HObject::post(Event &&e) {
  eventQueue_.post(std::move(e));
  notify();
}

bool HObject::tryPopAndExecute() { return eventQueue_.tryPopAndExecute(); }

bool HObject::hasEvents() { return !eventQueue_.empty(); }

void HObject::subscribe(std::function<void()> cb) {
  std::lock_guard<std::mutex> lock(mtx_);
  subscriber_ = cb;
}

void HObject::unsubscribe() {
  std::lock_guard<std::mutex> lock(mtx_);
  subscriber_ = nullptr;
}

void HObject::notify() {
  std::lock_guard<std::mutex> lock(mtx_);
  if (subscriber_)
    subscriber_();
}

} // namespace helios::core
