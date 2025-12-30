#include "timesys/timers_manager.hpp"

#include <future>
#include <gtest/gtest.h>
#include <vector>

/**
 * @brief Verifies that one timer fires.
 */
TEST(TimersManagerTest, OneTimerFires) {
  helios::timesys::TimersManager t;
  std::promise<std::chrono::steady_clock::time_point> pr;
  std::chrono::steady_clock::time_point start =
      std::chrono::steady_clock::now();
  t.create(std::chrono::seconds(1), [&] {
    pr.set_value(std::chrono::steady_clock::now());
  });
  std::chrono::steady_clock::time_point finish = pr.get_future().get();
  EXPECT_GE(finish - start, std::chrono::seconds(1));
}

/**
 * @brief Verifies that multiple timers fire.
 *
 * @details
 * - Multiple timers shall be created and each of them shall be verified that it
 *   has fired.
 */
TEST(TimersManagerTest, MultipleTimersFire) {
  struct TriggeredTimer {
    std::chrono::steady_clock::time_point start;
    std::promise<std::chrono::steady_clock::time_point> pr;
  };
  helios::timesys::TimersManager t;
  std::vector<std::shared_ptr<TriggeredTimer>> triggeredTimers;
  for (int i{}; i < 10; ++i) {
    auto timer = std::make_shared<TriggeredTimer>();
    timer->start = std::chrono::steady_clock::now();
    t.create(std::chrono::milliseconds(10), [timer] {
      timer->pr.set_value(std::chrono::steady_clock::now());
    });
    triggeredTimers.push_back(timer);
  };
  for (auto const &timer : triggeredTimers) {
    std::chrono::steady_clock::time_point finish = timer->pr.get_future().get();
    EXPECT_GE(finish - timer->start, std::chrono::milliseconds(10));
  }
}
