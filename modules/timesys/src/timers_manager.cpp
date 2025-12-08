#include "timesys/timers_manager.hpp"

#include <queue>

#include "core/active_h_object.hpp"

namespace helios::timesys {

class TimersManager::Impl : public core::ActiveHObject {
public:
private:
}; // class Impl

TimersManager::TimersManager()
    : impl_{std::make_unique<TimersManager::Impl>()} {}

TimersManager::~TimersManager() = default;

void TimersManager::create(Duration duration, Callback cb) {}

} // namespace helios::timesys
