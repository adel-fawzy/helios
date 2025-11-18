#pragma once

#include "core/event_queue/interface.hpp"
#include <gmock/gmock.h>

namespace helios::mocks::core::event_queue {

class MockEventQueue : public helios::core::event_queue::Interface {
public:
  MOCK_METHOD(void, pushImpl, (const helios::core::Event &event), (override));
  MOCK_METHOD(void, handle, (), (override));
};

} // namespace helios::mocks::core::event_queue
