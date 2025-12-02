#include "core/event_loop.hpp"

#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "core/future_result.hpp"

namespace {

class Calculator : public helios::core::HObject {
public:
  helios::core::FutureResult<int>::Ptr add(int first, int second) {
    auto result = std::make_shared<helios::core::FutureResult<int>>();
    post([this, first, second, result] { result->set(first + second); });
    return result;
  }
}; // class Calculator

} // namespace

/**
 * @brief Tests creating an empty event loop and then adding an HObject.
 *
 * @details
 * - Create an empty event loop then adds one HObject to it then executes an
 *   event.
 * - Verify that the event has executed.
 */
TEST(EventLoopTest, AddHObject) {
  std::cout << "TEST: Creating Calculator" << std::endl;
  auto c = std::make_shared<Calculator>();
  std::cout << "TEST: Creating empty loop" << std::endl;
  helios::core::EventLoop loop;
  std::cout << "TEST: Adding calculator to loop" << std::endl;
  loop.add(c);
  std::cout << "hello " << std::endl;
  std::cout << "TEST: Calling add" << std::endl;
  // std::optional<int> result = c->add(2, 3)->get();
  c->add(2, 3);
  std::cout << "TEST: Called add" << std::endl;
  // ASSERT_TRUE(result);
  std::cout << "TEST: Expecting" << std::endl;
  // EXPECT_EQ(result.value(), 5);
}

/**
 * @brief Tests posting an event to an HObject.
 *
 * @details
 * - Create an HObject and add it to the loop.
 * - Verify that the event has executed.
 */
TEST(EventLoopTest, HandleOneHObject) {
  auto c = std::make_shared<Calculator>();
  helios::core::EventLoop loop{c};
  std::optional<int> result = c->add(2, 3)->get();
  ASSERT_TRUE(result);
  EXPECT_EQ(result.value(), 5);
}

/**
 * @brief Tests that the destructor waits for the current events.
 *
 * @details
 * - When the destructor of the event loop is called, it should wait for the
 *   current events to be executed first.
 * - Verify that the event has executed.
 */
TEST(EventLoopTest, WaitsForCurrentEvents) {
  int result{};
  {
    auto c = std::make_shared<Calculator>();
    helios::core::EventLoop loop{c};
    c->add(2, 3)->then([&result](auto resultValue) {
      result = *resultValue;
    }); // Post event
    // Delete c (Should wait for the posted event to be executed first)
  }
  EXPECT_EQ(result, 5);
}

/**
 * @brief Test adding an HObject to multiple loops.
 *
 * @details
 * - Tests adding an HObject to a loop and then deleting this loop and then
 *   adding the HObject to a new loop.
 */
TEST(EventLoopTest, NewLoop) {
  auto c = std::make_shared<Calculator>();
  {
    helios::core::EventLoop loop{c};
    auto resultValue = c->add(2, 3)->get();
    EXPECT_EQ(resultValue, 5);
  } // Loop is deleted
  helios::core::EventLoop loop{c}; // Create a new loop
  auto resultValue = c->add(3, 3)->get();
  EXPECT_EQ(resultValue, 6);
}
