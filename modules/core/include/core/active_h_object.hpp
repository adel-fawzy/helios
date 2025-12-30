#pragma once

#include <condition_variable>
#include <deque>
#include <thread>

#include "event.hpp"
#include "future_result.hpp"
#include "h_object.hpp"

namespace helios::core {

#define REQ(RETURN_TYPE, BODY)                                         \
  [&]() -> helios::core::FutureResult<RETURN_TYPE>::Ptr {                      \
    auto fut = std::make_shared<helios::core::FutureResult<RETURN_TYPE>>();    \
    post([=]() mutable BODY);                                                  \
    return fut;                                                                \
  }()

#define REQ_CALLABLE(RETURN_TYPE, FUNC)                                \
  [&]() -> helios::core::FutureResult<RETURN_TYPE>::Ptr {                      \
    auto fut = std::make_shared<helios::core::FutureResult<RETURN_TYPE>>();    \
    post([=, FUNC = FUNC]() mutable { FUNC(fut); });                           \
    return fut;                                                                \
  }()

/**
 * @class core::ActiveHObject
 *
 * @brief Adds active asynchronous capability to HObject.
 *
 * @details
 * - Provides the derived classes with asynchronous event handling using the
 *   post() function.
 * - During construction, the thread which runs the event queue is created.
 * - During destruction, all events in the queue are executed first and then the
 *   object is destroyed.
 *
 * @note
 * - All public functions are asynchronous except the post() function which must
 *   be synchronous since it is communicating with the event queue.
 * - All public functions are thread-safe.
 */
class ActiveHObject : public HObject {
public:
  /**
   * @brief Constructor.
   *
   * @param hBus Optional pointer to the signal bus.
   *
   * @note
   * - Blocks until the event is started and then returns.
   */
  ActiveHObject(HBus *hBus = nullptr);

  /**
   * @brief Destructor.
   *
   * @note
   * - Blocks until all events in the queue are handled.
   */
  ~ActiveHObject() override;

  /**
   * @brief Delete copy and move semantics.
   */
  ActiveHObject(const ActiveHObject &) = delete;
  ActiveHObject &operator=(const ActiveHObject &) = delete;
  ActiveHObject(ActiveHObject &&) = delete;
  ActiveHObject &operator=(ActiveHObject &&) = delete;

protected:
  /**
   * @brief Posts an event to the queue.
   *
   * @tparam EventT Type of event to be posted.
   * @param e Event to be posted.
   */
  template <typename EventT>
  void post(EventT &&e) {
    postImpl(std::forward<EventT>(e));
  }

private:
  /**
   * @brief Loop thread that runs the event queue.
   */
  std::thread t_;

  /**
   * @brief Set to true to stop the thread.
   */
  bool stopLoop_{false};

  /**
   * @brief Synchronizes between the loop thread and other threads.
   */
  std::condition_variable cv_;

  /**
   * @brief Protects this class.
   */
  std::mutex mtx_;

  /**
   * @brief Event queue.
   */
  std::deque<Event> q_;

  /**
   * @brief Function that runs in the loop thread.
   */
  void run();

  /**
   * @brief Posts an event to the queue.
   *
   * @param e Event to be posted.
   */
  void postImpl(Event e);
}; // class ActiveHObject

} // namespace helios::core
