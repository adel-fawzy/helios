#include "long_running_thread.hpp"

#include <gtest/gtest.h>
#include <exception>
#include <chrono>
#include <future>
#include <thread>
#include <memory>

TEST(LongRunningThreadTest, WorkIsDone)
{
  bool isWorkDone{false};
  std::promise<void> pr;
  helios::core::LongRunningThread t{
      // WORK
      [&]
      {
        isWorkDone = true;
        pr.set_value();
      },
      // PREDICATE
      []
      {
        return true;
      }};
  pr.get_future().wait();
  EXPECT_TRUE(isWorkDone);
}

TEST(LongRunningThreadTest, DestructorStopsThreadImmediately)
{
  std::mutex mtx;
  std::unique_lock<std::mutex> lock(mtx);
  std::condition_variable cv;
  bool isObjectDestroyed{false};
  std::thread t = std::thread([&]
                              {
                                auto t = std::make_shared<helios::core::LongRunningThread>(
                                    // WORK
                                    [&]
                                    {
                                    },
                                    // PREDICATE
                                    []
                                    { return false; });
                                t.reset(); // ~LongRunningThread is called and should not block for long
                                std::unique_lock<std::mutex> lock(mtx);
                                isObjectDestroyed = true;
                                cv.notify_one();
                                lock.unlock(); });
  // Wait for the object to be destroyed for up to 1 sec
  bool ok = cv.wait_for(lock, std::chrono::seconds(5),
                        [&]
                        { return isObjectDestroyed; });
  // Fail test if timeout happened
  EXPECT_TRUE(ok);
  t.join();
}

TEST(LongRunningThreadTest, WorkThrows)
{
  helios::core::LongRunningThread t{
      // WORK
      []
      {
        throw std::runtime_error("Runtime error while doing some work!");
      },
      // PREDICATE
      []
      { return false; }};
}
