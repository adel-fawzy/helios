#pragma once

#include "core/isignal_bus.hpp"
#include <gmock/gmock.h>

namespace helios::mocks::core {

class MockSignalBus : public helios::core::ISignalBus {
public:
  MOCK_METHOD(void, unsubscribe, (helios::core::ID id), (override));
  MOCK_METHOD(void, subscribeImpl,
              (helios::core::ID id, std::type_index signalType, RawCallback cb),
              (override));
  MOCK_METHOD(void, publishImpl,
              (std::type_index signalType, const void *signal), (override));
};

} // namespace helios::mocks::core::signal_bus
