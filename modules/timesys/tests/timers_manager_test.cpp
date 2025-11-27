#include "timesys/timers_manager.hpp"

#include <future>
#include <gtest/gtest.h>

/**
 * @test
 * @brief Tests creating one timer and waiting for it to fire.
 *
 * @steps
 * 1. Call 'TimersManager::create' with a 10 milli-second duration.
 * 2. Wait until the timer fires.
 * 3. Verify that the timer fired by checking that a bool is toggled.
 */
TEST(TimersManagerTest, NormalTimer) {
  helios::timesys::TimersManager t;
  std::promise<void> pr;
  bool isTimerFired{false};
  t.create(std::chrono::milliseconds(10), [&] {
    isTimerFired = true; // Indicate that the timer fired
    pr.set_value();
  });
  pr.get_future().wait(); // Wait for the timer to fire
  EXPECT_TRUE(isTimerFired);
}
