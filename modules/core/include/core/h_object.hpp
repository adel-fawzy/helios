#pragma once

#include <memory>

#include "event_queue.hpp"
#include "signal_bus.hpp"

namespace helios::core {

/**
 * @class core::HObject
 *
 * @brief Provides its derived classes asynchronous event handling.
 *
 * @details
 * - The main functionality of this class is that it lets its derived classes
 *   handle their events asynchronously using the post() function.
 * - It has a unique ID that is used when listening to and unlistening from
 *   signals on the signal bus.
 * - It can live with or without a signal bus.
 * - The derived classes can use listen() and unlisten() functions to
 *   communicate on the bus.
 *
 * @note
 * - All public functions are synchronous.
 * - All public functions are thread-safe.
 */
class HObject {
public:
  /**
   * @brief Constructor.
   *
   * @param signalBus Optional shared pointer to the signal bus.
   */
  HObject(std::shared_ptr<SignalBus> signalBus = nullptr);

  /**
   * @brief Virtual destructor.
   */
  virtual ~HObject();

  /**
   * @brief Handles the first event in the event queue and then returns true.
   *
   * @details
   * - If there are no events in the event queue, then it will immediately
   *   return false.
   * - If an event is handled, it will be popped from the event queue so that it
   *   is not handled again.
   *
   * @return true if an event is handled, false otherwise.
   */
  bool tryPopAndExecute();

  /**
   * @brief Checks if there is at least one event in the event queue.
   *
   * @return true if there is at least one event in the event queue, false
   *         otherwise.
   */
  bool hasEvents();

protected:
  /**
   * @brief Adds an event to the event queue.
   *
   * @param e Event to be added.
   */
  void post(const Event &e);
  void post(Event &&e);

  /**
   * @brief Listens to a signal type.
   *
   * @tparam SignalT The signal type to listen to.
   * @tparam Callback The type of the callback function.
   * @param cb The callback function to invoke when the signal is published.
   */
  template <typename SignalT, typename Callback> void listen(Callback &&cb) {
    if (signalBus_)
      signalBus_->listen<SignalT>(id_, std::forward<Callback>(cb));
  }

  /**
   * @brief Publishes a signal to the signal bus.
   *
   * @tparam SignalT The signal type to publish.
   * @param s The signal instance to publish.
   */
  template <typename SignalT> void publish(SignalT s) {
    if (signalBus_)
      signalBus_->publish<SignalT>(s);
  }

private:
  /**
   * @brief ID of the next HObject to be created.
   */
  static inline ID nextId_{0};

  /**
   * @brief Event queue of the HObject.
   */
  EventQueue eventQueue_;

  /**
   * @brief Shared pointer to the signal bus.
   */
  std::shared_ptr<SignalBus> signalBus_;

  /**
   * @brief Unique HObject ID.
   */
  const ID id_;
}; // class HObject

} // namespace helios::core
