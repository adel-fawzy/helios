#pragma once

#include <initializer_list>
#include <memory>

#include "h_object.hpp"

namespace helios::core {

/**
 * @class core::EventLoop
 *
 * @brief Runs one or more HObjects in a separate thread.
 *
 * @details
 * - When run() is called, will keep on running all events from all HObjects.
 * - When stop() is called, will stop running the events from all HObjects.
 * - If there are no events in any HObject, will sleep until the first event in
 *   any HObbject is added.
 *
 * @note
 * - All public functions are synchronous.
 * - All public functions are not thread-safe.
 */
class EventLoop {
public:
  /**
   * @brief Constructor.
   */
  EventLoop();

  /**
   * @brief Destructor.
   */
  ~EventLoop();

  /**
   * @brief Delete copy and move semantics.
   */
  EventLoop(const EventLoop &) = delete;
  EventLoop &operator=(const EventLoop &) = delete;
  EventLoop(EventLoop &&) = delete;
  EventLoop &operator=(EventLoop &&) = delete;

  /**
   * @brief Adds an HObject to the loop.
   *
   * @param obj Shared pointer to HObject to be added.
   */
  void add(std::shared_ptr<HObject> obj);

  /**
   * @brief Adds more than one HObject to the loop.
   *
   * @param objs List of shared pointers to HObjects.
   */
  void add(std::initializer_list<std::shared_ptr<HObject>> objs);

  /**
   * @brief Runs all HObjects in a separate thread.
   *
   * @details
   * - Blocks until the thread is actually started.
   * - Silently returns if the thread is already running.
   */
  void run();

  /**
   * @brief Stops the thread and closes it.
   *
   * @details
   * - Handles all events of HObjects that were added to them before the call to
   *   this function.
   * - Then stops the thread and closes it.
   * - Blocks until the thread is closed.
   */
  void stop();

private:
  /**
   * @brief Forward decleration for the implementation class.
   */
  class Impl;

  /**
   * @brief Unique pointer to the implementation class.
   */
  std::unique_ptr<Impl> impl_;
}; // class EventLoop

} // namespace helios::core
