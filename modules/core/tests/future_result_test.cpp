#include "core/future_result.hpp"

#include <future>
#include <gtest/gtest.h>
#include <thread>

#include "core/active_h_object.hpp"

namespace {

class Calculator : helios::core::ActiveHObject {
public:
  helios::core::FutureResult<int>::Ptr add(int first, int second) {
    auto handleAdd = [first, second](auto fut) { fut->set(first + second); };
    return REQ_CALLABLE(int, handleAdd);
  };
}; // class Calculator

class Client : public helios::core::ActiveHObject {
public:
  Client(std::shared_ptr<Calculator> calculator) : calculator_(calculator) {}
  std::future<int> start_1(int first, int second) {
    calculator_->add(first, second) THEN_POST({ pr_.set_value(*result); });
    return pr_.get_future();
  }
  std::future<int> start_2(int first, int second) {
    auto pr = std::make_shared<std::promise<int>>();
    std::future<int> fut = pr->get_future();
    auto cb = [this, pr](std::shared_ptr<int> result) mutable {
      pr->set_value(*result);
    };
    calculator_->add(first, second) THEN_POST_CALLABLE(cb);
    return fut;
  }

private:
  std::promise<int> pr_;
  std::shared_ptr<Calculator> calculator_;
}; // class Client

} // namespace

/**
 * @brief Use macros to pass a callback.
 */
TEST(FutureResultTest, Macros) {
  auto c = std::make_shared<Calculator>();
  Client client(c);
  std::future<int> fut_1 = client.start_1(5, 4);
  EXPECT_EQ(fut_1.get(), 9);
  std::future<int> fut_2 = client.start_2(4, 9);
  EXPECT_EQ(fut_2.get(), 13);
}

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
  result.then(
      [&resultValue](std::shared_ptr<int> value) { resultValue = *value; });
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
