#pragma once

#include <atomic>
#include <memory>

#include "signal_bus.hpp"

namespace helios::core {

/**
 * @class core::HObject
 *
 * @brief Provides access to the SignalBus through a unique ID.
 *
 * @details
 * - Each HObject has a unique ID. This ID is used to listen on the SignalBus.
 * - Unsubscribes from the SignalBus through its unique ID during destruction.
 */
class HObject {
public:
  /**
   * @brief Constructor.
   *
   * @param bus Shared pointer to the signal bus.
   */
  HObject(std::shared_ptr<SignalBus> bus) : bus_(bus), id_(++nextId_) {}

  /**
   * @brief Virtual destructor.
   */
  virtual ~HObject() {
    if (bus_)
      bus_->unlisten(id_);
  }

  /**
   * @brief Delete copy and move semantics.
   */
  HObject(const HObject &) = delete;
  HObject &operator=(const HObject &) = delete;
  HObject(HObject &&) = delete;
  HObject &operator=(HObject &&) = delete;

protected:
  /**
   * @brief Listens to a signal type.
   *
   * @tparam SignalT The signal type to listen to.
   * @tparam CallbackT The type of the callback function.
   * @param cb The callback function to invoke when the signal is published.
   */
  template <typename SignalT, typename CallbackT> void listen(CallbackT &&cb) {
    post([this, cb = std::forward<CallbackT>(cb)] {
      if (bus_)
        bus_->listen<SignalT>(id_, std::move(cb));
    });
  }

  /**
   * @brief Publishes a signal to the signal bus.
   *
   * @tparam SignalT The signal type to publish.
   * @param s The signal to publish.
   */
  template <typename SignalT> void publish(SignalT &&s) {
    post([this, s = std::forward<SignalT>(s)] {
      if (bus_)
        bus_->publish<SignalT>(std::move(s));
    });
  }

private:
  /**
   * @brief ID of the next HObject to be created.
   */
  static inline std::atomic<ID> nextId_{0};

  /**
   * @brief Unique ID.
   */
  const ID id_;

  /**
   * @brief Shared pointer to the signal bus.
   */
  std::shared_ptr<SignalBus> bus_;
}; // class HObject

} // namespace helios::core
