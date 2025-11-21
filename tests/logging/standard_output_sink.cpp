#include "logging/standard_output_sink.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

#include "core/event_queue.hpp"

namespace helios::logging::sink {

class Test_StandardOutputSink_Env : public ::testing::Test {
public:
  /**
   * @brief Saves the buffer of std::cout and then replaces it with '_buffer'.
   */
  void SetUp() override {
    eventQueue_ = std::make_shared<core::EventQueue>();
    instanceUnderTest_ = std::make_shared<StandardOutputSink>(eventQueue_);
    originalBuffer_ = std::cout.rdbuf();
    std::cout.rdbuf(buffer_.rdbuf());
  }

  /**
   * @brief Restores the original buffer of std::cout.
   */
  void TearDown() override { std::cout.rdbuf(originalBuffer_); }

  /**
   * @brief Wraps 'StandardOutputSink::write'.
   */
  void write(const std::string &msg) const {
    instanceUnderTest_->write(std::move(msg));
    eventQueue_->handle();
  }

  /**
   * @brief Returns the value inside the buffer.
   */
  std::string getResult() const { return buffer_.str(); }

private:
  /**
   * @brief Event queue of the instance under test.
   */
  std::shared_ptr<core::EventQueue> eventQueue_;

  /**
   * @brief Instance under test.
   */
  std::shared_ptr<StandardOutputSink> instanceUnderTest_;

  /**
   * @brief Temp buffer.
   */
  std::ostringstream buffer_;

  /**
   * @brief Original buffer.
   */
  std::streambuf *originalBuffer_;
};

/**
 * @test
 * @brief Test that when a message is passed to 'StandardOutputSink::write',
 *        it is stored in the buffer.
 *
 * @steps
 * 1. Create a message.
 * 2. Call 'StandardOutputSink::write' and pass the created message.
 * 3. Assert that the buffer of the sink has the expected message.
 */
TEST_F(Test_StandardOutputSink_Env, MessageIsStoredInBuffer) {
  std::string msg{"Hello World!\n"};
  write(msg);
  ASSERT_EQ(getResult(), msg);
}

/**
 * @test
 * @brief Test that when an empty message is passed to
 *        'StandardOutputSink::write', it is stored in the buffer.
 *
 * @steps
 * 1. Create an empty message.
 * 2. Call 'StandardOutputSink::write' and pass the created message.
 * 3. Assert that the buffer of the sink has the expected message.
 */
TEST_F(Test_StandardOutputSink_Env, EmptyMessageIsStoredInBuffer) {
  std::string msg{""};
  write(msg);
  ASSERT_EQ(getResult(), msg);
}

} // namespace helios::logging::sink
