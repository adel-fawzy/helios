#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <thread>

#include "src/long_running_thread.hpp"

TEST(LongRunningThreadTest, StartAndStop) {
  helios::core::LongRunningThread t;
  t.start(
      // WORK
      [] {
        std::this_thread::sleep_for(
            std::chrono::seconds(5)); // Simulate long work
      },
      // PREDICATE
      [] { return false; });
  t.stop();
}

TEST(LongRunningThreadTest, StartAndDestruct) {
  helios::core::LongRunningThread t;
  t.start(
      // WORK
      [] {
        std::this_thread::sleep_for(
            std::chrono::seconds(5)); // Simulate long work
      },
      // PREDICATE
      [] { return false; });
}

TEST(LongRunningThreadTest, StopWhileNotRunning) {
  helios::core::LongRunningThread t;
  t.stop();
}

TEST(LongRunningThreadTest, DestructWhileNotRunning) {
  helios::core::LongRunningThread t;
}

TEST(LongRunningThreadTest, PredicateWakesThread) {
  helios::core::LongRunningThread t;
  std::atomic<bool> predicateResult{false};
  t.start(
      // WORK
      [] {},
      // PREDICATE
      [&predicateResult] { return predicateResult.load(); });
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  predicateResult = true;
  t.notify();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
