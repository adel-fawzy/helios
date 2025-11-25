#include <gtest/gtest.h>

#include "core/h_object.hpp"

namespace {

class Calculator : public helios::core::HObject {
public:
  void add(int first, int second, std::function<void(int)> callback) {
    post([this, first, second, callback] { callback(first + second); });
  }
}; // class Calculator

struct Sig {
  Sig(int v = 0) : value{v} {}
  int value{};
  bool operator==(const Sig &other) const { return value == other.value; }
}; // struct Sig

inline std::ostream &operator<<(std::ostream &os, const Sig &s) {
  return os << s.value;
}

class Listener : public helios::core::HObject {
public:
  Listener(std::shared_ptr<helios::core::SignalBus> bus,
           std::function<void(std::shared_ptr<const Sig>)> cb)
      : HObject{bus}, cb_{cb} {
    listen<Sig>([this](auto value) {
      auto e = [this, value] { cb_(value); };
      post(e);
    });
  }

private:
  std::function<void(std::shared_ptr<const Sig>)> cb_;
}; // class Listener

class Publisher : public helios::core::HObject {
public:
  Publisher(std::shared_ptr<helios::core::SignalBus> bus) : HObject{bus} {}
  void startPublish(Sig s) { publish<Sig>(std::move(s)); }
}; // class Publisher

} // namespace

TEST(HObjectTest, PostOneEvent) {
  auto c = std::make_shared<Calculator>();
  int actual{};
  c->add(2, 3, [&actual](auto result) { actual = result; });
  c->tryPopAndExecute();
  EXPECT_EQ(actual, 5);
}

TEST(HObjectTest, PostMultipleEvents) {
  auto c = std::make_shared<Calculator>();
  for (int i{}; i < 50; ++i) {
    int expected{i + i + 1};
    int actual{};
    c->add(i, i + 1, [&actual](auto result) { actual = result; });
    c->tryPopAndExecute();
    EXPECT_EQ(actual, expected);
  }
}

TEST(HObjectTest, ListenToASignal) {
  auto bus = std::make_shared<helios::core::SignalBus>();
  Sig expected{3};
  auto actual = std::make_shared<const Sig>();
  Listener l{bus, [&actual](auto s) { actual = s; }};
  Publisher p{bus};
  p.startPublish(expected);
  l.tryPopAndExecute();
  EXPECT_EQ(*actual, expected);
}

TEST(HObjectTest, ListenToMultipleSignals) {
  for (int i{}; i < 50; ++i) {
    auto bus = std::make_shared<helios::core::SignalBus>();
    Sig expected{i};
    auto actual = std::make_shared<const Sig>();
    Listener l{bus, [&actual](auto s) { actual = s; }};
    Publisher p{bus};
    p.startPublish(expected);
    l.tryPopAndExecute();
    EXPECT_EQ(*actual, expected);
  }
}
