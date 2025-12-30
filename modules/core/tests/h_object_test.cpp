#include "core/h_object.hpp"

#include <gtest/gtest.h>

namespace {

struct Speed {
  Speed() = default;
  Speed(int value) : value(value) {}
  int value;
  bool operator==(const Speed &other) const { return value == other.value; }
}; // struct Speed

inline std::ostream &operator<<(std::ostream &os, const Speed &s) {
  return os << s.value;
}

class Listener : public helios::core::HObject {
public:
  Listener(helios::core::HBus *hBus) : HObject(hBus) {}
  void startListening(std::function<void(std::shared_ptr<const Speed>)> cb) {
    auto onSig = [cb](auto sig) { cb(sig); };
    listen<Speed>(onSig);
  }
}; // class Listener

class Publisher : public helios::core::HObject {
public:
  Publisher(helios::core::HBus *hBus) : HObject(hBus) {}
  void startPublishing(Speed s) { PUBLISH(s); }
}; // class Publisher

class Subject : public helios::core::HObject {
public:
  Subject(helios::core::HBus *hBus = nullptr) : HObject(hBus) {}

  void subscribeToSpeed(
      helios::core::ID observerId,
      std::function<void(std::shared_ptr<const Speed>)> observerCallback
  ) {
    addSubscriber<Speed>(observerId, std::move(observerCallback));
  }

  void notify(Speed s) { notifySubscribers(s); }
}; // class Subject

class Observer : public helios::core::HObject {
public:
  Observer(std::shared_ptr<Subject> subject)
      : HObject(nullptr), subject_(subject) {
    subject->subscribeToSpeed(id_, [this](std::shared_ptr<const Speed> sig) {
      speed_ = *sig;
    });
    subscribeTo(subject);
  }

  Speed getSpeed() const { return speed_; }

private:
  std::shared_ptr<Subject> subject_;
  Speed speed_;
}; // class Observer

} // namespace

/**
 * @brief Publishes a signal then listens to it and verifies that the signal is
 *        received correctly.
 */
TEST(HObjectTest, ListenToSignal) {
  helios::core::HBus hBus{};
  Listener l(&hBus);
  Speed result;
  l.startListening([&result](std::shared_ptr<const Speed> s) { result = *s; });
  Speed expected(66);
  Publisher p(&hBus);
  p.startPublishing(expected);
  EXPECT_EQ(result, expected);
}

/**
 * @brief An observer subscribes to a subject. The subject notifies the observer
 *        with a signal. The test verifies that the observer received the signal
 *        correctly.
 */
TEST(HObjectTest, SubscriberNotification) {
  auto subject = std::make_shared<Subject>();
  Observer observer(subject);
  Speed expected(99);
  subject->notify(expected);
  Speed result = observer.getSpeed();
  EXPECT_EQ(result, expected);
}
