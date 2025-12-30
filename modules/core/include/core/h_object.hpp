#pragma once

#include "h_bus.hpp"
#include <atomic>

#include <mutex>

namespace helios::core {

#define LISTEN(TYPE, BODY)                                                     \
  listen<TYPE>([this](auto sig) mutable { post([ this, sig ] BODY); })

#define LISTEN_CALLABLE(TYPE, FUNC)                                            \
  listen<TYPE>([this](auto sig) mutable { post(FUNC); })

#define PUBLISH(VALUE) publish(VALUE)

#define SUBSCRIBE(HOBJECT, SIGNAL, BODY)                                       \
  HOBJECT->subscribeTo##SIGNAL(id_, [this](auto sig) mutable {                 \
    post([ this, sig ] BODY);                                                  \
  });                                                                          \
  subscribeTo(HOBJECT);

#define SUBSCRIBE_CALLABLE(HOBJECT, SIGNAL, FUNC)                              \
  HOBJECT->subscribeTo##SIGNAL(id_, [this, FUNC](auto sig) mutable {           \
    post([=, F = FUNC]() mutable { F(sig); });                                 \
  });                                                                          \
  subscribeTo(HOBJECT);

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
    // Unlisten from the signal bus
    if (hBus_)
      hBus_->unlisten(id_);
    // Remove this object from subscribed objects
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto const &[id, weakObj] : subscribedObjects_) {
      if (auto obj = weakObj.lock()) {
        obj->removeSubscriber(id_);
      }
    }
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

  /**
   * @brief Adds a new subscriber to a certain signal.
   *
   * @tparam SignalT Type of the signal to subscribe to.
   * @param subscriberId ID of the HObject that wants to subscribe.
   * @param subscriberCallback Callback that will be called to notify the
   *                           subscriber.
   */
  template <typename SignalT>
  void addSubscriber(
      ID subscriberId,
      std::function<void(std::shared_ptr<const SignalT>)> subscriberCallback
  ) {
    // Wrap user callback
    auto wrapper = [cb = std::move(subscriberCallback)](
                       std::shared_ptr<const void> s
                   ) { cb(std::static_pointer_cast<const SignalT>(s)); };

    std::lock_guard<std::mutex> lock(mtx_);
    signalsMap_[typeid(SignalT)][subscriberId] = std::move(wrapper);
  }

  /**
   * @brief Removes a subscriber by its ID.
   *
   * @param subscriberId ID of the subscriber to remove.
   */
  void removeSubscriber(ID subscriberId) {
    std::lock_guard<std::mutex> lock(mtx_);

    for (auto &[type, subscribersMap] : signalsMap_) {
      subscribersMap.erase(subscriberId);
    }
  }

  /**
   * @brief Notifies the subscribers of a certain signal.
   *
   * @tparam SignalT Type of the signal.
   * @param s The signal value.
   */
  template <typename SignalT>
  void notifySubscribers(SignalT &&s) {
    using T = std::remove_cv_t<std::remove_reference_t<SignalT>>;
    std::unordered_map<ID, std::function<void(std::shared_ptr<const void>)>>
        snapshot;

    {
      std::lock_guard<std::mutex> lock(mtx_);
      auto it = signalsMap_.find(typeid(T));
      if (it != signalsMap_.end())
        snapshot = it->second;
    }

    for (auto &[id, cb] : snapshot)
      cb(std::make_shared<T>(s));
  }

  void subscribeTo(std::shared_ptr<HObject> obj) {
    if (!obj)
      return;

    {
      std::lock_guard<std::mutex> lock(mtx_);
      subscribedObjects_[obj->id_] = obj;
    }
  }

  /**
   * @brief Unique ID.
   */
  const ID id_;

private:
  /**
   * @brief ID of the next HObject to be created.
   */
  static inline std::atomic<ID> nextId_{0};

  /**
   * @brief Pointer to the signal bus.
   */
  HBus *hBus_;

  /**
   * @brief Type alias for the callback of the subscribers.
   */
  using Callback = std::function<void(std::shared_ptr<const void>)>;

  /**
   * @brief Type alias for subscribers map.
   */
  using SubscribersMap = std::unordered_map<ID, Callback>;

  /**
   * @brief Type alias for the signals map.
   */
  using SignalsMap = std::unordered_map<std::type_index, SubscribersMap>;

  /**
   * @brief Holds the subscribers for each signal.
   */
  SignalsMap signalsMap_;

  /**
   * @brief HObjects subscribed to.
   */
  std::unordered_map<ID, std::weak_ptr<HObject>> subscribedObjects_;

  /**
   * @brief Mutex to protect the class.
   */
  std::mutex mtx_;
}; // class HObject

} // namespace helios::core
