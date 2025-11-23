#include "core/module.hpp"

#include <gtest/gtest.h>

#include "core/mock_event_queue.hpp"
#include "core/mock_signal_bus.hpp"

namespace helios::core {

// Create a class to expose the protected functions of the class under test
class ModuleUnderTest : public Module {
public:
  ModuleUnderTest(std::shared_ptr<core::IEventQueue> eventQueue,
                  std::shared_ptr<core::SignalBus> signalBus)
      : Module(eventQueue, signalBus) {}
  ~ModuleUnderTest() override = default;
  using Module::add;
  using Module::publishSignal;
  using Module::subscribeSignal;
};

/**
 * @test
 * @brief Test adding new events.
 *
 * @details
 * Test that when 'ModuleUnderTest::add' is called,
 * 'event_queue::Interface::pushImpl' is called with the correct argument.
 *
 * @steps
 * 1. Create 'MockEventQueue'.
 * 2. Create an empty event.
 * 3. Expect that 'MockEventQueue::pushImpl' is called with any argument.
 * 4. Let 'MockEventQueue::pushImpl' save its argument (event) in the empty event
 *    created earlier.
 * 5. Create 'MockSignalBus'.
 * 6. Create 'ModuleUnderTest' and pass the mock objects.
 * 7. Call 'ModuleUnderTest::add' with any event.
 * 8. Assert that the empty event is true.
 */
TEST(TestModule, AddEvent) {
  auto queueMockPtr =
      std::make_shared<mocks::core::event_queue::MockEventQueue>();
  Event e{};
  EXPECT_CALL(*queueMockPtr, pushImpl(testing::_))
      .WillOnce(testing::SaveArg<0>(&e));
  auto signalBusMockPtr =
      std::make_shared<mocks::core::MockSignalBus>();
  ModuleUnderTest m(queueMockPtr, signalBusMockPtr);
  m.add(Event{[]() {}});
  ASSERT_TRUE(static_cast<bool>(e));
}

/**
 * @test
 * @brief Subscribing to signals from the signal bus.
 *
 * @details
 * Test that when 'ModuleUnderTest::subscribeSignal' is called,
 * 'signal_bus::Interface::subscribe' is called with the correct argument.
 *
 * @steps
 * 1. Create 'MockEventQueue'.
 * 2. Create 'MockSignalBus'.
 * 3. Create an empty signal.
 * 4. Expect that 'MockSignalBus::subscribe' is called with any arguments.
 * 5. Let 'MockSignalBus::subscribe' save its argument (signal) in the empty
 *    signal created earlier.
 * 6. Create 'ModuleUnderTest' and pass the mock objects.
 * 7. Call 'ModuleUnderTest::subscribeSignal' with any signal.
 * 8. Assert that the empty signal is true.
 */
TEST(TestModule, SubscribeToSignal) {
  auto queueMockPtr =
      std::make_shared<mocks::core::event_queue::MockEventQueue>();
  auto signalBusMockPtr =
      std::make_shared<mocks::core::MockSignalBus>();
  struct Speed {
  } s;
  std::type_index savedSignalType = typeid(void);
  EXPECT_CALL(*signalBusMockPtr,
              subscribeImpl(testing::_, testing::_, testing::_))
      .WillOnce(testing::SaveArg<1>(&savedSignalType));

  ModuleUnderTest m(queueMockPtr, signalBusMockPtr);
  m.subscribeSignal<Speed>([](const Speed &) {});
  ASSERT_TRUE(savedSignalType == typeid(Speed));
}

/**
 * @test
 * @brief Publishing signals to the signal bus.
 *
 * @details
 * Test that when 'ModuleUnderTest::publishSignal' is called,
 * 'signal_bus::Interface::publish' is called with the correct argument.
 *
 * @steps
 * 1. Create 'MockEventQueue'.
 * 2. Create 'MockSignalBus'.
 * 3. Create an empty signal.
 * 4. Expect that 'MockSignalBus::publish' is called with any arguments.
 * 5. Let 'MockSignalBus::publish' save its argument (signal) in the empty
 *    signal created earlier.
 * 6. Create 'ModuleUnderTest' and pass the mock objects.
 * 7. Call 'ModuleUnderTest::publishSignal' with any signal.
 * 8. Assert that the empty signal is true.
 */
TEST(TestModule, PublishSignal) {
  auto queueMockPtr =
      std::make_shared<mocks::core::event_queue::MockEventQueue>();
  auto signalBusMockPtr =
      std::make_shared<mocks::core::MockSignalBus>();
  struct Speed {
  } s;
  std::type_index savedSignalType = typeid(void);
  EXPECT_CALL(*signalBusMockPtr, publishImpl(testing::_, testing::_))
      .WillOnce(testing::SaveArg<0>(&savedSignalType));

  ModuleUnderTest m(queueMockPtr, signalBusMockPtr);
  m.publishSignal<Speed>(s);
  ASSERT_TRUE(savedSignalType == typeid(Speed));
}

/**
 * @test
 * @brief Unsubscribe from subscribed signals during destruction.
 *
 * @details
 * Test that when 'ModuleUnderTest::~ModuleUnderTest' is called,
 * 'signal_bus::Interface::unsubscribe' is called with ID 1.
 *
 * @steps
 * 1. Create 'MockEventQueue'.
 * 2. Create 'MockSignalBus'.
 * 3. Create an empty signal.
 * 4. Expect that 'MockSignalBus::unsubscribe' is called with ID 1.
 * 5. Create 'ModuleUnderTest' and pass the mock objects.
 * 6. Delete the 'ModuleUnderTest' instance.
 */
TEST(TestModule, UnsubscribeOnDestruction) {
  auto queueMockPtr =
      std::make_shared<mocks::core::event_queue::MockEventQueue>();
  auto signalBusMockPtr =
      std::make_shared<mocks::core::MockSignalBus>();
  EXPECT_CALL(*signalBusMockPtr, unsubscribe(1));
  ModuleUnderTest m(queueMockPtr, signalBusMockPtr);
}

} // namespace helios::core
