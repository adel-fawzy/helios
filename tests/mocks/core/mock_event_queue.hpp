#pragma once

#include "core/ievent_queue.hpp"
#include <gmock/gmock.h>

namespace helios::mocks::core::event_queue {

class MockEventQueue : public helios::core::IEventQueue {
public:
  MOCK_METHOD(void, pushImpl, (helios::core::Event event), (override));
  MOCK_METHOD(void, handle, (), (override));
};

} // namespace helios::mocks::core::event_queue
