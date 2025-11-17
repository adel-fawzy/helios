#include "interface.hpp"

#include <condition_variable>
#include <mutex>

namespace helios::core::event_queue {

/**
 * @class EventQueue::EventQueue
 *
 * @brief Concrete implementation of the event loop interface.
 */
class EventQueue : public Interface {
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

  void push(Event event) override;
  void handle() override;

private:
  /**
   * @brief Queue of events.
   */
  std::queue<Event> _queue;

  /**
   * @brief Mutex to protect this class.
   */
  std::mutex _mtx;

  /**
   * @brief Condition variable to signal when an event is available in the
   * queue.
   */
  std::condition_variable _cv;
};

} // namespace helios::core::event_queue
