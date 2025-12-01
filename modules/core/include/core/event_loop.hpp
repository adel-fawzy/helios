#pragma once

#include <initializer_list>
#include <memory>

#include "h_object.hpp"

namespace helios::core
{

  /**
   * @class core::EventLoop
   *
   * @brief Runs one or more HObjects in a separate thread.
   *
   * @details
   * - During construction, creates a new thread which runs the HObject(s).
   * - During destruction, waits first for the current events to be executed and then returns.
   */
  class EventLoop
  {
  public:
    /**
     * @brief Constructor.
     *
     * @param obj HObject.
     */
    EventLoop(std::shared_ptr<HObject> obj);

    /**
     * @brief Constructor
     *
     * @param HObjects.
     */
    EventLoop(std::initializer_list<std::shared_ptr<HObject>> objs);

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

  private:
    /**
     * @brief Forward declaration for the implementation class.
     */
    class Impl;

    /**
     * @brief Unique pointer to the implementation class.
     */
    std::unique_ptr<Impl> impl_;
  }; // class EventLoop

} // namespace helios::core
