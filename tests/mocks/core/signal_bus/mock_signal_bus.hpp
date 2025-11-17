#pragma once

#include "core/signal_bus/interface.hpp"
#include <gmock/gmock.h>

namespace helios::mocks::core::signal_bus {

class MockSignalBus : public helios::core::signal_bus::Interface {
public:
  MOCK_METHOD(void, unsubscribe, (helios::core::ID id), (override));
  MOCK_METHOD(void, subscribeImpl,
              (helios::core::ID id, std::type_index signalType, RawCallback cb),
              (override));
  MOCK_METHOD(void, publishImpl,
              (std::type_index signalType, const void *signal), (override));
};

} // namespace helios::mocks::core::signal_bus
