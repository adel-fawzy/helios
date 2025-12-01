#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace helios::core
{

  /**
   * @class core::LongRunningThread
   *
   * @brief Runs long running work for the client.
   *
   * @details
   * - This class allows the client to do some long running work in a separate thread.
   * - This class takes two functions from the client:
   *   - Work: This function is called to run work for the client. It should not be long running.
   *   - Predicate: This function is called to determine whether there is more work to do for the client or not.
   * - This class calls the client's work function to run work for it in a separate thread.
   * - After the work function returns, it calls the predicate.
   * - If the predicate function returns true, then the work function is called again.
   * - If the predicate function returns false, then the thread sleeps.
   * - The client can call the notify() function to wake up the thread.
   * - When the thread is woken up, it calls predicate to determine whether to sleep again or run the work function.
   * - The work function must not block. It should do the work and then return.
   * - The class captures exceptions from the work function.
   * - The predicate function must not throw any exceptions.
   * 
   * @note
   * - All public functions are synchronous.
   * - All public functions are not thread-safe.
   */
  class LongRunningThread final
  {
  public:
    /**
     * @brief Type alias for the client's work function.
     */
    using WorkFunc = std::function<void()>;

    /**
     * @brief Type alias for the client's predicate function.
     */
    using PredicateFunc = std::function<bool()>;

    /**
     * @brief Constructor
     *
     * @param w The client's work function.
     * @param p The client's predicate function.
     *
     * @details
     * - Returns after the thread is started.
     */
    LongRunningThread(WorkFunc w, PredicateFunc p);

    /**
     * @brief Destructor.
     */
    virtual ~LongRunningThread();

    /**
     * @brief Delete copy and move semantics.
     */
    LongRunningThread(const LongRunningThread &) = delete;
    LongRunningThread &operator=(const LongRunningThread &) = delete;
    LongRunningThread(LongRunningThread &&) = delete;
    LongRunningThread &operator=(LongRunningThread &&) = delete;

    /**
     * @brief Wakes the thread.
     *
     * @details
     * - If the thread was sleeping, it will wake up and call the client's predicate to determine whether to sleep again or
     *   call the work function.
     */
    void notify();

  private:
    /**
     * @brief Client's work function.
     */
    WorkFunc w_;

    /**
     * @brief Client's predicate function.
     */
    PredicateFunc p_;

    /**
     * @brief Thread manager.
     */
    std::thread t_;

    /**
     * @brief Used to wake the thread.
     */
    std::condition_variable cv_;

    /**
     * @brief Protects this class.
     */
    std::mutex mtx_;

    /**
     * @brief Set to true to stop the thread.
     */
    bool stop_{false};
  }; // class LongRunningThread

}; // namespace helios::core
