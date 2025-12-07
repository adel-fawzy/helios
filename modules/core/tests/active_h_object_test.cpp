#include "core/active_h_object.hpp"

#include <future>
#include <gtest/gtest.h>
#include <thread>

#include "core/future_result.hpp"

namespace {

class Calculator : public helios::core::ActiveHObject {
public:
  helios::core::FutureResult<int>::Ptr add(int first, int second) {
    auto result = std::make_shared<helios::core::FutureResult<int>>();
    post([this, first, second, result] { result->set(first + second); });
    return result;
  }
}; // class Calculator

class Order : public helios::core::ActiveHObject {
public:
  void postInOrder(std::vector<int> &v) {
    for (int i{0}; i < 10; ++i)
      post([i, &v] { v.push_back(i); });
  }
}; // class Order

class Thrower : public helios::core::ActiveHObject {
public:
  void throwException() {
    post([] {
      throw std::runtime_error("Testing that the HObject catches exceptions");
    });
  }
}; // class Thrower

} // namespace

/**
 * @brief Tests that posted functions are being executed.
 *
 * @details
 * - Verifies that ActiveHObject runs posted events.
 */
TEST(ActiveHObjectTest, PostedEventRuns) {
  Calculator c;
  helios::core::FutureResult<int>::Ptr res = c.add(3, 4);
  std::optional<int> res_value = res->get();
  ASSERT_NE(res_value, std::nullopt);
  EXPECT_EQ(*res_value, 7);
}

/**
 * @brief ActiveHObject catches exceptions.
 *
 * @details
 * - Verifies that ActiveHObject captures exceptions thrown from events.
 */
TEST(ActiveHObjectTest, HObjectCatchesExceptions) {
  Thrower t;
  t.throwException();
}

/**
 * @brief ActiveHObject runs events in order.
 */
TEST(ActiveHObjectTest, ExecutesEventsInOrder) {
  Order obj;
  std::vector<int> result;
  obj.postInOrder(result);
  while (result.size() != 10) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  for (int i{0}; i < 10; ++i)
    EXPECT_EQ(result[i], i);
}
