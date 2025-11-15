#include "Interface.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>

namespace helios::EventQueue {

/**
 * @class EventQueue::DefaultEventQueue
 */
class DefaultEventQueue : public Interface {
public:
  DefaultEventQueue() = default;
  ~DefaultEventQueue() override = default;

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

} // namespace helios::EventQueue
