#include "core/h_object.hpp"

#include <gtest/gtest.h>

namespace {

struct Speed {
  Speed() = default;
  Speed(int value) : value(value) {}
  int value;
  bool operator==(const Speed &other) const { return value == other.value; }
}; // struct Speed

class Listener : public helios::core::HObject {
public:
  Listener(std::shared_ptr<helios::core::HBus> hBus) : HObject(hBus) {}
  void startListening(std::function<void(std::shared_ptr<const Speed>)> cb) {
    auto onSig = [cb](auto sig) { cb(sig); };
    listen<Speed>(onSig);
  }
}; // class Listener

class Publisher : public helios::core::HObject {
public:
  Publisher(std::shared_ptr<helios::core::HBus> hBus) : HObject(hBus) {}
  void startPublishing(Speed s) { PUBLISH(s); }
}; // class Publisher

} // namespace

/**
 * @brief Publishes a signal then listens to it and verifies that the signal is
 *        received correctly.
 */
TEST(HObjectTest, ListenToSignal) {
  auto hBus = std::make_shared<helios::core::HBus>();
  Listener l(hBus);
  Speed result;
  l.startListening([&result](std::shared_ptr<const Speed> s) { result = *s; });
  Speed expected(66);
  Publisher p(hBus);
  p.startPublishing(expected);
  EXPECT_EQ(result, expected);
}
