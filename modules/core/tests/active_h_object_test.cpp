#include "core/active_h_object.hpp"

#include <future>
#include <gtest/gtest.h>
#include <iostream>

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
    {
      std::promise<void> pr;
      std::future<void> fut = pr.get_future();
      post([&] {
        v.push_back(1);
        pr.set_value();
      });
      fut.get();
    }
    {
      std::promise<void> pr;
      std::future<void> fut = pr.get_future();
      post([&] {
        v.push_back(2);
        pr.set_value();
      });
      fut.get();
    }
    {
      std::promise<void> pr;
      std::future<void> fut = pr.get_future();
      post([&] {
        v.push_back(3);
        pr.set_value();
      });
      fut.get();
    }
  }
}; // class Thrower

class Thrower : public helios::core::ActiveHObject {
public:
  void throwException() {
    post([] {
      std::cout << "Event called" << std::endl;
      throw std::runtime_error("Testing that the HObject catches exceptions");
    });
  }
}; // class Thrower

}; // namespace

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
  std::cout << "TEST: Calling" << std::endl;
  t.throwException();
}

/**
 * @brief ActiveHObject runs events in order.
 */
TEST(ActiveHObjectTest, ExecutesEventsInOrder) {
  Order obj;
  std::vector<int> result;
  obj.postInOrder(result);
  EXPECT_EQ(result[0], 1);
  EXPECT_EQ(result[1], 2);
  EXPECT_EQ(result[2], 3);
}
