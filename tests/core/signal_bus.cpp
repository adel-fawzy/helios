#include "core/signal_bus.hpp"

#include <gtest/gtest.h>

namespace {

/**
 * @brief A random ID.
 */
constexpr helios::core::ID RANDOM_ID{42};

} // namespace

namespace helios::core {

/**
 * @test
 * @brief Test normal scenario.
 *
 * @details
 * Test normal scenario where 'SignalBus::listen' is called to listen to a
 * signal and then 'SignalBus::publish' is called for the same signal. Then
 * 'SignalBus::unlisten' is called.
 *
 * @steps
 * 1. Create a random ID and an int variable initialized to zero.
 * 2. Create a callback that increments the int variable.
 * 3. Create a 'SignalBus' and call 'SignalBus::listen' to listen to any
 *    signal type.
 * 4. Call 'SignalBus::publish' for the same signal type.
 * 5. Expect that the int variable is equal to one.
 * 6. Repeat steps 4 and 5 to ensure that publishing works multiple times.
 * 7. Call 'SignalBus::unlisten' to unregister the ID.
 */
TEST(TestSignalBus, NormalScenario) {
  ID id{RANDOM_ID};
  int callbackInvocationCount{0};
  auto callback = [&callbackInvocationCount](auto s) {
    (void)s; // Unused parameter
    ++callbackInvocationCount;
  };
  SignalBus bus;
  using Speed = struct {};
  bus.listen<Speed>(id, callback);
  for (auto i : {1, 2}) {
    bus.publish<Speed>(Speed{});
    EXPECT_EQ(callbackInvocationCount, i);
  }
  bus.unlisten(id);
}

/**
 * @test
 * @brief Test publishing a signal that has no listeners.
 *
 * @details
 * Test if 'SignalBus::publish' is called on a signal that does not have
 * listeners.
 *
 * @steps
 * 1. Create a 'SignalBus' and call 'SignalBus::publish'.
 */
TEST(TestSignalBus, PublishSignalWithNoListeners) {
  SignalBus bus;
  using Temperature = struct {};
  bus.publish<Temperature>(Temperature{});
}

/**
 * @test
 * @brief Test unlistening.
 *
 * @details
 * Test that after unlistening from a signal, the callback is not called.
 *
 * @steps
 * 1. Create a random ID and an int variable initialized to zero.
 * 2. Create a callback that increments the int variable.
 * 3. Create a 'SignalBus' and call 'SignalBus::unlisten' to listen to any
 *    signal type.
 * 4. Call 'SignalBus::publish' for the same signal type.
 * 5. Expect that the int variable is equal to one.
 * 6. Call 'SignalBus::unlisten' to unlisten the ID.
 * 7. Call 'SignalBus::publish' for the same signal type.
 * 8. Expect that the int variable is still equal to one.
 */
TEST(TestSignalBus, Unlistening) {
  SignalBus bus;
  ID id{RANDOM_ID};
  int callbackInvocationCount{0};
  auto callback = [&callbackInvocationCount](auto s) {
    (void)s; // Unused parameter
    ++callbackInvocationCount;
  };
  using Speed = struct {};
  bus.listen<Speed>(id, callback);
  bus.publish<Speed>(Speed{});
  EXPECT_EQ(callbackInvocationCount, 1);
  bus.unlisten(id);
  bus.publish<Speed>(Speed{});
  EXPECT_EQ(callbackInvocationCount, 1);
}

/**
 * @test
 * @brief Test unlistening if the ID is not listening to any signal.
 *
 * @details
 * Test if 'SignalBus::unlisten' is called on an ID that is not listening to
 * any signal.
 *
 * @steps
 * 1. Create a 'SignalBus' and call 'SignalBus::unlisten' with an ID that is
 *    not registered.
 */
TEST(TestSignalBus, UnlistenIfIDNotRegistered) {
  SignalBus bus;
  ID id{RANDOM_ID};
  bus.unlisten(id);
}

} // namespace helios::core
