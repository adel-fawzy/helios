#pragma once

#include <atomic>

#include "h_bus.hpp"

namespace helios::core {

#define LISTEN(TYPE, BODY)                                                     \
  listen<TYPE>([this](auto sig) mutable { post([ this, sig ] BODY); })

#define LISTEN_CALLABLE(TYPE, FUNC)                                            \
  listen<TYPE>([this](auto sig) mutable { post(FUNC); })

#define PUBLISH(VALUE) publish(VALUE)

/**
 * @class core::HObject
 *
 * @brief Provides access to the HBus through a unique ID.
 *
 * @details
 * - Each HObject has a unique ID. This ID is used to listen on the HBus.
 * - Unsubscribes from the HBus through its unique ID during destruction.
 */
class HObject {
public:
  /**
   * @brief Constructor.
   *
   * @param bus Pointer to the signal bus.
   */
  HObject(HBus *hBus) : hBus_(hBus), id_(++nextId_) {}

  /**
   * @brief Virtual destructor.
   */
  virtual ~HObject() {
    if (hBus_)
      hBus_->unlisten(id_);
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
  template <typename SignalT, typename CallbackT>
  void listen(CallbackT &&cb) {
    if (hBus_)
      hBus_->listen<SignalT>(id_, std::forward<CallbackT>(cb));
  }

  /**
   * @brief Publishes a signal to the signal bus.
   *
   * @tparam SignalT The signal type to publish.
   * @param s The signal to publish.
   */
  template <typename SignalT>
  void publish(SignalT &&s) {
    if (hBus_)
      hBus_->publish<SignalT>(std::forward<SignalT>(s));
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
   * @brief Pointer to the signal bus.
   */
  HBus *hBus_;
}; // class HObject

} // namespace helios::core
