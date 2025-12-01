#include "core/future_result.hpp"

#include <future>
#include <gtest/gtest.h>
#include <thread>

/**
 * @brief Client takes the result from its callback.
 *
 * @details
 * - The test thread shall take the result from its callback.
 * - The callback shall verify the expected result.
 */
TEST(FutureResultTest, Callback) {
  helios::core::FutureResult<int> result;
  int resultValue{};
  result.then([&resultValue](std::shared_ptr<int> value) {
    resultValue = *value;
  });
  const int expected{5};
  std::thread t{[&result, expected] { result.set(expected); }};
  t.join();
  EXPECT_EQ(resultValue, expected);
}

/**
 * @brief Client takes the result by calling get() before the result is
 *        available.
 *
 * @details
 * - The test thread shall take the result by calling get() before the result is
 *   available.
 * - The result shall be verified.
 */
TEST(FutureResultTest, CallGetBeforeResultIsAvailable) {
  helios::core::FutureResult<int> result;
  int resultValue{};
  const int expected{5};
  // Create the consumer thread that will call get()
  std::promise<void> consumerStarted;
  std::thread consumer{[&resultValue, &result, &consumerStarted] {
    consumerStarted.set_value(); // Indicate that the consumer has started
    auto value = result.get();   // Block on the result and get it
    if (value)
      resultValue = *value;
  }};
  consumerStarted.get_future().get(); // Wait till the consumer thread starts
  // Create the producer thread that will set the result value
  std::thread producer{[&result, expected] { result.set(expected); }};
  // Wait for all threads to finish
  producer.join();
  consumer.join();
  // Expect
  EXPECT_EQ(resultValue, expected);
}

/**
 * @brief Client takes the result by calling get() after the result becomes
 *        available.
 *
 * @details
 * - The test thread shall take the result by calling get() after the result is
 *   available.
 * - The result shall be verified.
 */
TEST(FutureResultTest, CallGetAfterResultIsAvailable) {
  helios::core::FutureResult<int> result;
  int resultValue{};
  const int expected{5};
  // Create the producer thread that will set the result value
  std::thread producer{[&result, expected] { result.set(expected); }};
  producer.join(); // Wait for the producer to set the value
  // Create the consumer thread that will call get()
  std::thread consumer{[&resultValue, &result] {
    auto value = result.get(); // Block on the result and get it
    if (value)
      resultValue = *value;
  }};
  consumer.join(); // Wait for the consumer thread to store the result
  // Expect
  EXPECT_EQ(resultValue, expected);
}

/**
 * @brief get() timeouts.
 *
 * @details
 * - The test thread shall give get() a timeout period and the the timeout shall
 *   be triggered.
 * - The result shall be verified that it is nullptr.
 */
TEST(FutureResultTest, GetReturnsDueToTimeout) {
  helios::core::FutureResult<int> result;
  auto value = result.get(std::chrono::milliseconds(100));
  EXPECT_EQ(value, std::nullopt);
}

/**
 * @brief getPtr() timeouts.
 *
 * @details
 * - The test thread shall give getPtr() a timeout period and the the timeout
 *   shall be triggered.
 * - The result shall be verified that it is nullptr.
 */
TEST(FutureResultTest, GetPtrReturnsDueToTimeout) {
  helios::core::FutureResult<int> result;
  auto value = result.getPtr(std::chrono::milliseconds(100));
  EXPECT_EQ(value, nullptr);
}
