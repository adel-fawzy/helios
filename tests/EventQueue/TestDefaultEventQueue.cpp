#include "EventQueue/DefaultEventQueue.hpp"
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace {

/**
 * @brief One second duration.
 */
constexpr std::chrono::seconds ONE_SECOND{1};

/**
 * @brief Five second duration.
 */
constexpr std::chrono::seconds FIVE_SECONDS{5};

} // namespace

namespace helios::EventQueue {
/**
 * @test
 * @brief  Tests the normal scenario.
 *
 * @details
 * Tests the normal scenario where an event is pushed and then handled.
 *
 * @steps
 * 1. Create a flag and initialize it with false.
 * 2. Create an event that sets the previous flag.
 * 3. Call 'DefaultEventQueue::push' and pass to it the created event.
 * 4. Call 'DefaultEventQueue::handle'.
 * 5. Assert that the flag is set to true.
 */
TEST(TestDefaultEventQueue, TestNormalScenario) {
  bool flag{false};
  auto event = [&flag] { flag = true; };
  DefaultEventQueue queue;
  queue.push(event);
  queue.handle();
  ASSERT_TRUE(flag);
}

/**
 * @test
 * @brief  Tests the normal scenario but with two events.
 *
 * @details
 * Tests the normal scenario where two events are pushed and then they are
 * handled.
 *
 * @steps
 * 1. Create two flags and initialize them with false.
 * 2. Create two events, one for each flag.
 * 3. Call 'DefaultEventQueue::push' two times for each events.
 * 4. Call 'DefaultEventQueue::handle' two times.
 * 5. Assert that both flags are set to true.
 */
TEST(TestDefaultEventQueue, TestNormalScenarioWithTwoEvents) {
  bool flag1{false};
  bool flag2{false};
  auto event1 = [&flag1] { flag1 = true; };
  auto event2 = [&flag2] { flag2 = true; };
  DefaultEventQueue queue;
  queue.push(event1);
  queue.push(event2);
  queue.handle();
  queue.handle();
  ASSERT_TRUE(flag1);
  ASSERT_TRUE(flag2);
}

/**
 * @test
 * @brief  Tests that the class blocks until an event is pushed.
 *
 * @details
 * Tests that the class blocks until an event is pushed.
 *
 * @steps
 * 1. Create a condition variable and a mutex.
 * 2. Create a thread and pass to it a function that calls
 *    'DefaultEventQueue::handle'. The function should also set a flag and
 *    notify the condition variable when 'DefaultEventQueue::handle' returns.
 * 3. Start the thread.
 * 4. Sleep for one second.
 * 5. Assert that the thread is still joinable.
 * 6. Create an atomic flag and initialize it with false.
 * 7. Create an event that sets the previous flag.
 * 8. Call 'DefaultEventQueue::push' and pass to it the created event.
 * 9. Use the condition variable to wait for max five seconds until the thread finishes.
 * 10. Assert that the flag is set to true.
 * 11. Join the thread.
 */
TEST(TestDefaultEventQueue, TestBlockingWhenThereAreNoEventsInTheQueue) {
  DefaultEventQueue queue;
  std::condition_variable cv;
  std::mutex mtx;
  std::unique_lock<std::mutex> lock{mtx};
  bool isThreadFinished{false};
  std::thread handlerThread{[&queue, &isThreadFinished, &cv, &mtx] {
    std::lock_guard<std::mutex> lock(mtx);
    queue.handle();
    isThreadFinished = true;
    cv.notify_one();
  }};

  // Sleep for one second to ensure that the handler thread is blocked
  std::this_thread::sleep_for(ONE_SECOND);
  ASSERT_TRUE(handlerThread.joinable());
  std::atomic<bool> flag{false};
  queue.push([&flag] { flag.store(true); });

  // Wait for max 5 secs until the thread finishes
  cv.wait_for(lock, FIVE_SECONDS,
              [&isThreadFinished] { return isThreadFinished; });

  ASSERT_TRUE(flag.load());
  handlerThread.join();
}

} // namespace helios::EventQueue
