#include "core/h_loop.hpp"

#include <gtest/gtest.h>

#include "core/in_active_h_object.hpp"

namespace {

class Calculator : public helios::core::InActiveHObject {
public:
  Calculator(std::shared_ptr<helios::core::HLoop> loop)
      : helios::core::InActiveHObject(loop) {}
  helios::core::FutureResult<int>::Ptr add(int first, int second) {
    auto result = std::make_shared<helios::core::FutureResult<int>>();
    post([this, first, second, result] { result->set(first + second); });
    return result;
  }
}; // class Calculator

class Worker : public helios::core::InActiveHObject {
public:
  Worker(std::shared_ptr<helios::core::HLoop> loop)
      : helios::core::InActiveHObject(loop) {}
  void work(std::shared_ptr<std::vector<int>> v) {
    post([v] { v->push_back(1); });
  }
}; // class Worker

} // namespace

/**
 * @brief One object is added to HLoop and executed one event.
 *
 * @details
 * - Verifies that one event is executed successfully on an InActiveHObject that
 *   is added to an HLoop.
 */
TEST(HLoopTest, OneEventExecutes) {
  auto loop = std::make_shared<helios::core::HLoop>();
  Calculator c(loop);
  std::shared_ptr<helios::core::FutureResult<int>> fut = c.add(3, 7);
  std::optional<int> res = fut->get();
  ASSERT_NE(res, std::nullopt);
  EXPECT_EQ(*res, 10);
}

/**
 * @brief Multiple objects are added to HLoop and their events are executed
 *        correctly.
 *
 * @details
 * - Verifies that the events of multiple objects are executed successfully from
 *   HLoop.
 */
TEST(HLoopTest, MultipleEventsExecute) {
  auto loop = std::make_shared<helios::core::HLoop>();
  // Create vector
  auto v = std::make_shared<std::vector<int>>();
  v->reserve(100);
  // Create 100 workers
  for (int i{}; i < 100; ++i) {
    auto w = std::make_shared<Worker>(loop);
    w->work(v);
  }
  while (v->size() != 100) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  EXPECT_EQ(v->size(), 100);
}
