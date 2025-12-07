#include "timesys/timers_manager.hpp"

#include <queue>

namespace helios::timesys {

class TimersManager::Impl {
public:
private:
}; // class Impl

TimersManager::TimersManager()
    : impl_{std::make_unique<TimersManager::Impl>()} {}

TimersManager::~TimersManager() = default;

void TimersManager::create(Duration duration, Callback cb) {}

} // namespace helios::timesys
