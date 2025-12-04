#pragma once

#include <memory>

#include "h_object.hpp"

namespace helios::core {

/**
 * @class core::ActiveHObject
 *
 * @brief Adds active event execution to HObject.
 */
class ActiveHObject : public HObject {
public:
  /**
   * @brief Constructor.
   *
   * @param signalBus Optional shared pointer to the signal bus.
   */
  ActiveHObject(std::shared_ptr<SignalBus> signalBus = nullptr);

  /**
   * @brief Default destructor.
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
  void onEventPosted() override;

private:
  /**
   * @brief Handles event execution.
   */
  std::thread t_;

  /**
   * @brief Set to true to stop the thread.
   */
  bool stopRunning_{false};

  /**
   * @brief Runs the queue until the object is destroyed.
   */
  void run();
}; // class ActiveHObject

}; // namespace helios::core
