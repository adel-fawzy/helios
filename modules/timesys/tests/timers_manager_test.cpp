#include "timesys/timers_manager.hpp"

#include <future>
#include <gtest/gtest.h>

/**
 * @brief Verifies that one timer fires.
 */
TEST(TimersManagerTest, OneTimerFires) {
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

/**
 * @brief Verifies that multiple timers fire.
 * 
 * @details
 * - Multiple timers shall be created and each of them shall be verified that it has fired.
 */
TEST(TimersManagerTest, MultipleTimersFire) {}
