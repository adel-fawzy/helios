#include <gtest/gtest.h>

#include "core/event_loop.hpp"
#include "core/h_object.hpp"
#include <chrono>
#include <iostream>
#include <thread>

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
  Listener(std::shared_ptr<helios::core::SignalBus> bus) : HObject{bus} {
    listen<Sig>(
        [this](auto value) { post([this, s = *value] { printSig(s); }); });
  }

private:
  void printSig(const Sig &s) {}

}; // class Listener

class Publisher : public helios::core::HObject {
public:
  Publisher(std::shared_ptr<helios::core::SignalBus> bus) : HObject{bus} {}
  void startPublish(Sig s) { publish<Sig>(std::move(s)); }
}; // class Publisher

} // namespace

TEST(EventLoopTest, HandleOneHObject) {
  helios::core::EventLoop loop;
  auto c = std::make_shared<Calculator>();
  loop.add(c);
  loop.run();
  int actual{};
  c->add(2, 3, [&actual](auto result) { actual = result; });
  loop.stop();
  EXPECT_EQ(actual, 5);
}

TEST(EventLoopTest, StartAndStopMultipleTimes) {
  auto sb = std::make_shared<helios::core::SignalBus>();
  auto pu = std::make_shared<Publisher>(sb);
  auto li = std::make_shared<Listener>(sb);
  helios::core::EventLoop el;
  el.add({pu, li});
  std::thread t{[&pu] {
    for (int i{}; i < 10; ++i) {
      pu->startPublish(i);
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }};

  el.run();
  std::this_thread::sleep_for(std::chrono::milliseconds(2));
  el.stop();

  el.run();
  std::this_thread::sleep_for(std::chrono::milliseconds(2));
  el.stop();

  el.run();
  t.join();
  el.run(); 
}
