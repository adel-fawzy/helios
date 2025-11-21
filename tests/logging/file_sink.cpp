#include "logging/file_sink.hpp"

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>

#include "core/event_queue.hpp"
#include "logging/file.hpp"

namespace {

/**
 * @brief File path used for testing.
 */
const std::string TEST_FILE_PATH{"test_file_sink_output"};

} // namespace

namespace helios::logging::sink {

class Test_FileSink_Env : public ::testing::Test {
public:
  /**
   * @brief Saves the buffer of std::cout and then replaces it with '_buffer'.
   */
  void SetUp() override {
    auto file = std::make_unique<File>(TEST_FILE_PATH);
    eventQueue_ = std::make_shared<core::EventQueue>();
    instanceUnderTest_ =
        std::make_shared<FileSink>(std::move(file), eventQueue_);
  }

  /**
   * @brief Deletes the test file.
   */
  void TearDown() override {
    std::filesystem::remove(TEST_FILE_PATH);
  }

  /**
   * @brief Wraps 'FileSink::write'.
   */
  void write(const std::string &msg) const {
    instanceUnderTest_->write(std::move(msg));
    eventQueue_->handle();
  }

  /**
   * @brief Returns the value inside the file.
   */
  std::string getResult() const {
    std::ifstream file(TEST_FILE_PATH);
    if (!file.is_open()) {
      return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }

private:
  /**
   * @brief Event queue of the instance under test.
   */
  std::shared_ptr<core::EventQueue> eventQueue_;

  /**
   * @brief Instance under test.
   */
  std::shared_ptr<FileSink> instanceUnderTest_;
};

/**
 * @test
 * @brief Test that when a message is passed to 'FileSink::write',
 *        it is stored in the given file.
 *
 * @steps
 * 1. Create a message.
 * 2. Call 'FileSink::write' and pass the created message.
 * 3. Assert that the file has the expected message.
 */
TEST_F(Test_FileSink_Env, MessageIsStoredInGivenFile) {
  std::string msg{"Hello World!\n"};
  write(msg);
  ASSERT_EQ(getResult(), msg);
}

/**
 * @test
 * @brief Test that when an empty message is passed to
 *        'FileSink::write', it is stored in the given file.
 *
 * @steps
 * 1. Create an empty message.
 * 2. Call 'FileSink::write' and pass the created message.
 * 3. Assert that the file has the expected message.
 */
TEST_F(Test_FileSink_Env, EmptyMessageIsStoredInGivenFile) {
  std::string msg{""};
  write(msg);
  ASSERT_EQ(getResult(), msg);
}

} // namespace helios::logging::sink
