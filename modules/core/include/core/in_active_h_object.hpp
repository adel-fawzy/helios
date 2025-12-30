#pragma once

#include "event.hpp"
#include "future_result.hpp"
#include "h_loop.hpp"
#include "h_object.hpp"

namespace helios::core {

/**
 * @class core::InActiveHObject
 *
 * @brief Adds passive asynchronous capability to HObject.
 *
 * @details
 * - Adds asynchronous capability but needs core::HLoop to run it.
 *
 * @note
 * - post() is thread-safe.
 */
class InActiveHObject : public HObject {
public:
  /**
   * @brief Constructor.
   *
   * @param loop Shared pointer to HLoop that will execute the events.
   * @param hBus Optional pointer to the signal bus.
   */
  InActiveHObject(std::shared_ptr<HLoop> loop,
                  HBus *hBus = nullptr);

  /**
   * @brief Default destructor.
   */
  ~InActiveHObject() override;

  /**
   * @brief Delete copy and move semantics.
   */
  InActiveHObject(const InActiveHObject &) = delete;
  InActiveHObject &operator=(const InActiveHObject &) = delete;
  InActiveHObject(InActiveHObject &&) = delete;
  InActiveHObject &operator=(InActiveHObject &&) = delete;

protected:
  /**
   * @brief Posts an event to the queue.
   *
   * @tparam EventT Type of event to be posted.
   * @param e Event to be posted.
   */
  template <typename EventT> void post(EventT &&e) {
    postImpl(std::forward<EventT>(e));
  }

private:
  /**
   * @brief Shared pointer to the event loop.
   */
  std::shared_ptr<HLoop> loop_;

  /**
   * @brief Posts an event to the queue.
   *
   * @param e Event to be posted.
   */
  void postImpl(Event e);
};

} // namespace helios::core
