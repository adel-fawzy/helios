#include "core/signal_bus/signal_bus.hpp"
#include <gtest/gtest.h>

namespace {

/**
 * @brief A random ID.
 */
constexpr helios::core::ID RANDOM_ID{42};

} // namespace

namespace helios::core::signal_bus {

/**
 * @test
 * @brief Test normal scenario.
 *
 * @details
 * Test normal scenario where 'SignalBus::subscribe' is called to subscribe to a
 * signal and then 'SignalBus::publish' is called for the same signal. Then
 * 'SignalBus::unsubscribe' is called.
 *
 * @steps
 * 1. Create a random ID and an int variable initialized to zero.
 * 2. Create a callback that increments the int variable.
 * 3. Create a 'SignalBus' and call 'SignalBus::subscribe' to subscribe to any
 *    signal type.
 * 4. Call 'SignalBus::publish' for the same signal type.
 * 5. Assert that the int variable is equal to one.
 * 6. Repeat steps 4 and 5 to ensure that publishing works multiple times.
 * 7. Call 'SignalBus::unsubscribe' to unsubscribe the ID.
 */
TEST(TestSignalBus, NormalScenario) {
  ID id{RANDOM_ID};
  int callbackInvocationCount{0};
  auto callback = [&callbackInvocationCount](auto &s) {
    (void)s; // Unused parameter
    ++callbackInvocationCount;
  };
  SignalBus bus;
  using Speed = struct {};
  bus.subscribe<Speed>(id, callback);
  for (auto i : {1, 2}) {
    bus.publish<Speed>(Speed{});
    ASSERT_EQ(callbackInvocationCount, i);
  }
  bus.unsubscribe(id);
}

/**
 * @test
 * @brief Test publishing a signal that has no subscribers.
 *
 * @details
 * Test if 'SignalBus::publish' is called on a signal that does not have
 * subscribers.
 *
 * @steps
 * 1. Create a 'SignalBus' and call 'SignalBus::publish'.
 */
TEST(TestSignalBus, PublishSignalWithNoSubscribers) {
  SignalBus bus;
  using Temperature = struct {};
  bus.publish<Temperature>(Temperature{});
}

/**
 * @test
 * @brief Test unsubscribing.
 *
 * @details
 * Test that after unsubscribing from a signal, the callback is not called.
 *
 * @steps
 * 1. Create a random ID and an int variable initialized to zero.
 * 2. Create a callback that increments the int variable.
 * 3. Create a 'SignalBus' and call 'SignalBus::subscribe' to subscribe to any
 *    signal type.
 * 4. Call 'SignalBus::publish' for the same signal type.
 * 5. Assert that the int variable is equal to one.
 * 6. Call 'SignalBus::unsubscribe' to unsubscribe the ID.
 * 7. Call 'SignalBus::publish' for the same signal type.
 * 8. Assert that the int variable is still equal to one.
 */
TEST(TestSignalBus, Unsubscribing) {
  SignalBus bus;
  ID id{RANDOM_ID};
  int callbackInvocationCount{0};
  auto callback = [&callbackInvocationCount](auto &s) {
    (void)s; // Unused parameter
    ++callbackInvocationCount;
  };
  using Speed = struct {};
  bus.subscribe<Speed>(id, callback);
  bus.publish<Speed>(Speed{});
  ASSERT_EQ(callbackInvocationCount, 1);
  bus.unsubscribe(id);
  bus.publish<Speed>(Speed{});
  ASSERT_EQ(callbackInvocationCount, 1);
}

/**
 * @test
 * @brief Test unsubscribing if the ID is not subscribed.
 *
 * @details
 * Test if 'SignalBus::unsubscribe' is called on an ID that is not subscribed to
 * any signal.
 *
 * @steps
 * 1. Create a 'SignalBus' and call 'SignalBus::unsubscribe' with an ID that is
 *    not subscribed.
 */
TEST(TestSignalBus, UnsubscribeIfIDNotSubscribed) {
  SignalBus bus;
  ID id{RANDOM_ID};
  bus.unsubscribe(id);
}

} // namespace helios::core::signal_bus
