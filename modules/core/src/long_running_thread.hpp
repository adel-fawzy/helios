#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <thread>

namespace helios::core {

/**
 * @class core::LongRunningThread
 *
 * @brief Runs a long-running function in a separate thread in a while loop.
 *
 * @details
 * - The function passed by the client will be run in a while loop.
 * - After the function returns, the class will sleep.
 * - The predicate passed by the client will be called to determine when should
 * the thread wake up.
 *
 * @note
 * - All public functions are synchronous.
 * - All public functions are thread-safe.
 */
class LongRunningThread final {
public:
  /**
   * @brief Default constructor.
   */
  LongRunningThread() = default;

  /**
   * @brief Destructor.
   */
  ~LongRunningThread();

  /**
   * @brief Delete copy and move semantics.
   */
  LongRunningThread(const LongRunningThread &) = delete;
  LongRunningThread &operator=(const LongRunningThread &) = delete;
  LongRunningThread(LongRunningThread &&) = delete;
  LongRunningThread &operator=(LongRunningThread &&) = delete;

  /**
   * @brief Starts the thread.
   *
   * @details
   * - Does not return immediately. Waits for the thread function to be executed
   *   and then returns.
   * - Returns silently if called while the thread is already running.
   *
   * @param work Function that the thread executes.
   * @param predicate Should return true when the thread should wake up,
   *                  otherwise should return false.
   */
  void start(std::function<void()> work, std::function<bool()> predicate);

  /**
   * @brief Stops the thread.
   *
   * @details
   * - Does not return immediately. Waits for the thread function to return and
   *   then returns.
   * - Returns silently if the thread is not running.
   */
  void stop();

  /**
   * @brief Wakes the thread if it is sleeping. When the thread is up, it will
   *        call the client's predicate to determine whether to sleep again or
   *        enter the while loop.
   */
  void notify();

private:
  /**
   * @brief Thread manager.
   */
  std::thread t_;

  /**
   * @brief Set to true to stop the thread. Reset to false once the thread is
   *        stopped.
   */
  std::atomic<bool> stopCommand_{false};

  /**
   * @brief Used to wake the thread.
   */
  std::condition_variable cv_;

  /**
   * @brief Protects this class.
   */
  std::mutex mtx_;

  /**
   * @brief Function that runs in the thread.
   *
   * @param work Function that performs the client's work.
   * @param predicate Function that returns true if the client has work to do.
   * @param promise Ref to promise which is set when the thread starts the
   *                function.
   */
  void loop(std::function<void()> work, std::function<bool()> predicate,
            std::promise<void> promise);
}; // class LongRunningThread

}; // namespace helios::core
