#include "core/ievent_queue.hpp"

#include <condition_variable>
#include <mutex>

namespace helios::core {

/**
 * @class EventQueue
 *
 * @brief Concrete implementation of the event loop interface.
 */
class EventQueue : public IEventQueue {
public:
  /**
   * @brief Default constructor.
   */
  EventQueue() = default;

  /**
   * @brief Default destructor.
   */
  ~EventQueue() override = default;

  /**
   * @brief Delete copy and move semantics.
   */
  EventQueue(const EventQueue &) = delete;
  EventQueue &operator=(const EventQueue &) = delete;
  EventQueue(EventQueue &&) = delete;
  EventQueue &operator=(EventQueue &&) = delete;

  void handle() override;

protected:
  void pushImpl(Event event) override;

private:
  /**
   * @brief Queue of events.
   */
  std::queue<Event> queue_;

  /**
   * @brief Mutex to protect this class.
   */
  std::mutex mtx_;

  /**
   * @brief Condition variable to signal when an event is available in the
   * queue.
   */
  std::condition_variable cv_;
}; // class EventQueue

} // namespace helios::core
