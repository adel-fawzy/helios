#include "core/event_loop.hpp"

#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>

#include "core/mock_event_queue.hpp"

namespace helios::core {

/**
 * @test
 * @brief Test normal scenario with 'EventLoop::~EventLoop'.
 *
 * @details
 * Test normal scenario where an event queue mock is passed to the event loop
 * and then the event loop is started. We will depend in this test on
 * 'EventLoop::~EventLoop' to stop the event loop.
 *
 * @steps
 * 1. Create an event queue mock.
 * 2. Expect that 'MockEventQueue::handle' will be called.
 * 3. Set the behavior of 'MockEventQueue::handle' such that it will notify a
 *    condition variable and set a flag to indicate it was called.
 * 4. Expect that 'MockEventQueue::pushImpl' will be called.
 * 5. Set the behavior of 'MockEventQueue::pushImpl' such that it will invoke the
 *    event immediately.
 * 6. Create an event loop with the event queue mock.
 * 7. Call 'EventLoop::start'.
 */
TEST(TestEventLoop, NormalScenarioWithDestructor) {
  auto eventQueueMockPtr =
      std::make_shared<mocks::core::event_queue::MockEventQueue>();
  std::mutex mtx;
  std::condition_variable cv;
  bool isCalled{false};
  EXPECT_CALL(*eventQueueMockPtr, handle())
      .Times(testing::AnyNumber())
      .WillRepeatedly([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        isCalled = true;
        cv.notify_one(); // Notify the testing thread
      });
  EXPECT_CALL(*eventQueueMockPtr, pushImpl(testing::_))
      .WillRepeatedly(
          testing::Invoke([](helios::core::Event event) { event(); }));

  EventLoop eventLoop{eventQueueMockPtr};

  eventLoop.start();
  // Wait until handle is called at least once
  {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&isCalled]() { return isCalled; });
    isCalled = false;
  }
}

/**
 * @test
 * @brief Test normal scenario with 'EventLoop::stop'.
 *
 * @details
 * Test normal scenario where an event queue mock is passed to the event loop
 * and then the event loop is started. We will depend in this test on
 * 'EventLoop::stop' to stop the event loop.
 *
 * @steps
 * 1. Create an event queue mock.
 * 2. Expect that 'MockEventQueue::handle' will be called.
 * 3. Set the behavior of 'MockEventQueue::handle' such that it will notify a
 *    condition variable and set a flag to indicate it was called.
 * 4. Expect that 'MockEventQueue::pushImpl' will be called.
 * 5. Set the behavior of 'MockEventQueue::pushImpl' such that it will invoke the
 *    event immediately.
 * 6. Create an event loop with the event queue mock.
 * 7. Call 'EventLoop::start'.
 * 8. Call 'EventLoop::stop'.
 */
TEST(TestEventLoop, NormalScenarioWithStop) {
  auto eventQueueMockPtr =
      std::make_shared<mocks::core::event_queue::MockEventQueue>();
  std::mutex mtx;
  std::condition_variable cv;
  bool isCalled{false};
  EXPECT_CALL(*eventQueueMockPtr, handle())
      .Times(testing::AnyNumber())
      .WillRepeatedly([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        isCalled = true;
        cv.notify_one(); // Notify the testing thread
      });
  EXPECT_CALL(*eventQueueMockPtr, pushImpl(testing::_))
      .WillRepeatedly(
          testing::Invoke([](helios::core::Event event) { event(); }));

  EventLoop eventLoop{eventQueueMockPtr};

  eventLoop.start();
  // Wait until handle is called at least once
  {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&isCalled]() { return isCalled; });
    isCalled = false;
  }
  eventLoop.stop();
}

/**
 * @test
 * @brief Test if 'EventLoop::start' is called when the event loop is already
 *        running.
 *
 * @steps
 * 1. Create an event queue mock.
 * 2. Expect that 'MockEventQueue::handle' will be called.
 * 3. Set the behavior of 'MockEventQueue::handle' such that it will notify a
 *    condition variable and set a flag to indicate it was called.
 * 4. Expect that 'MockEventQueue::pushImpl' will be called.
 * 5. Set the behavior of 'MockEventQueue::pushImpl' such that it will invoke the
 *    event immediately.
 * 6. Create an event loop with the event queue mock.
 * 7. Call 'EventLoop::start'.
 * 8. Call 'EventLoop::start'.
 */
TEST(TestEventLoop, StartWhileAlreadyRunning) {
  auto eventQueueMockPtr =
      std::make_shared<mocks::core::event_queue::MockEventQueue>();
  std::mutex mtx;
  std::condition_variable cv;
  bool isCalled{false};
  EXPECT_CALL(*eventQueueMockPtr, handle())
      .Times(testing::AnyNumber())
      .WillRepeatedly([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        isCalled = true;
        cv.notify_one(); // Notify the testing thread
      });
  EXPECT_CALL(*eventQueueMockPtr, pushImpl(testing::_))
      .WillRepeatedly(
          testing::Invoke([](helios::core::Event event) { event(); }));

  EventLoop eventLoop{eventQueueMockPtr};

  eventLoop.start();
  // Wait until handle is called at least once
  {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&isCalled]() { return isCalled; });
    isCalled = false;
  }
  eventLoop.start(); // Call start again
}

/**
 * @test
 * @brief Test if 'EventLoop::stop' is called when the event loop is not
 * running.
 *
 * @steps
 * 1. Create an event queue mock.
 * 2. Create an event loop with the event queue mock.
 * 3. Call 'EventLoop::stop'.
 */
TEST(TestEventLoop, StopWhileNotRunning) {
  auto eventQueueMockPtr =
      std::make_shared<mocks::core::event_queue::MockEventQueue>();
  EventLoop eventLoop{eventQueueMockPtr};
  eventLoop.stop();
}

/**
 * @test
 * @brief Test if the event loop can be started and stopped multiple times.
 *
 * @steps
 * 1. Create an event queue mock.
 * 2. Expect that 'MockEventQueue::handle' will be called.
 * 3. Set the behavior of 'MockEventQueue::handle' such that it will notify a
 *    condition variable and set a flag to indicate it was called.
 * 4. Expect that 'MockEventQueue::pushImpl' will be called.
 * 5. Set the behavior of 'MockEventQueue::pushImpl' such that it will invoke the
 *    event immediately.
 * 6. Create an event loop with the event queue mock.
 * 7. Call 'EventLoop::start'.
 * 8. Call 'EventLoop::stop'.
 * 9. Repeat steps 7 and 8 4 times (5 times in total).
 */
TEST(TestEventLoop, StartAndStopMultipleTimes) {
  auto eventQueueMockPtr =
      std::make_shared<mocks::core::event_queue::MockEventQueue>();
  std::mutex mtx;
  std::condition_variable cv;
  bool isCalled{false};
  EXPECT_CALL(*eventQueueMockPtr, handle())
      .Times(testing::AnyNumber())
      .WillRepeatedly([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        isCalled = true;
        cv.notify_one(); // Notify the testing thread
      });
  EXPECT_CALL(*eventQueueMockPtr, pushImpl(testing::_))
      .WillRepeatedly(
          testing::Invoke([](helios::core::Event event) { event(); }));

  EventLoop eventLoop{eventQueueMockPtr};

  for (int i = 0; i < 5; ++i) {
    eventLoop.start();
    // Wait until handle is called at least once
    {
      std::unique_lock<std::mutex> lock(mtx);
      cv.wait(lock, [&isCalled]() { return isCalled; });
      isCalled = false;
    }
    eventLoop.stop();
  }
}

} // namespace helios::core
