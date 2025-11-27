#pragma once

#include <chrono>
#include <functional>
#include <memory>

namespace helios::timesys {

/**
 * @class timesys::TimersManager
 *
 * @brief Creates a separate thread which could handle multiple timers.
 *
 * @details
 * - All public functions are asynchronous.
 * - All public functions are thread-safe.
 */
class TimersManager {
public:
  /**
   * @brief Constructor.
   */
  TimersManager();

  /**
   * @brief Destructor.
   */
  ~TimersManager();

  /**
   * @brief Delete copy and move semantics.
   */
  TimersManager(const TimersManager &) = delete;
  TimersManager &operator=(const TimersManager &) = delete;
  TimersManager(TimersManager &&) = delete;
  TimersManager &operator=(TimersManager &&) = delete;

  /**
   * @brief Type aliases.
   */
  using Clock = std::chrono::steady_clock;
  using Duration = Clock::duration;
  using TimePoint = Clock::time_point;
  using Callback = std::function<void()>;

  /**
   * @brief Creates a timer.
   *
   * @param duration The timer duration.
   * @param Callback The callback that is called when the duration is finished.
   */
  void create(Duration duration, Callback cb);

private:
  /**
   * @brief Forward decleration for the implementation class.
   */
  class Impl;

  /**
   * @brief Unique pointer to the implementation class.
   */
  std::unique_ptr<Impl> impl_;
}; // class TimersManager

}; // namespace helios::timesys
