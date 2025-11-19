#include "logging/sink/standard_output_sink.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

#include "core/event_queue/event_queue.hpp"

namespace helios::logging::sink {

class Test_StandardOutputSink_Env : public ::testing::Test {
public:
  /**
   * @brief Saves the buffer of std::cout and then replaces it with '_buffer'.
   */
  void SetUp() override {
    _eventQueue = std::make_shared<core::event_queue::EventQueue>();
    _instanceUnderTest = std::make_shared<StandardOutputSink>(_eventQueue);
    _originalBuffer = std::cout.rdbuf();
    std::cout.rdbuf(_buffer.rdbuf());
  }

  /**
   * @brief Restores the original buffer of std::cout.
   */
  void TearDown() override { std::cout.rdbuf(_originalBuffer); }

  /**
   * @brief Wrapper for 'StandardOutputSink::write'.
   */
  void write(const std::string &msg) const {
    _instanceUnderTest->write(std::move(msg));
    _eventQueue->handle();
  }

  /**
   * @brief Returns the value inside the buffer.
   */
  std::string getResult() const { return _buffer.str(); }

private:
  /**
   * @brief Event queue of the instance under test.
   */
  std::shared_ptr<core::event_queue::EventQueue> _eventQueue;

  /**
   * @brief Instance under test.
   */
  std::shared_ptr<StandardOutputSink> _instanceUnderTest;

  /**
   * @brief Temp buffer.
   */
  std::ostringstream _buffer;

  /**
   * @brief Original buffer.
   */
  std::streambuf *_originalBuffer;
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
