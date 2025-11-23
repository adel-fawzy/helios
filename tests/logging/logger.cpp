#include "Logger.hpp"

#include <gtest/gtest.h>

#include <thread>
namespace helios::logging {

TEST(TestLogger, Hi) {
  Logger logger{"ObjectName"};
  std::shared_ptr<core::IEventQueue> queue = loggerFactory.getEventQueue();
  LOG_DEBUG(logger) << "Adel Fawzy\n";
  queue->handle();
  queue->handle();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  LOG_INFO(logger) << "Aegir\n";
  std::this_thread::sleep_for(std::chrono::seconds(2));
  queue->handle();
  queue->handle();
  LOG_WARN(logger) << "Aegir lost cellular\n";
  std::this_thread::sleep_for(std::chrono::seconds(3));
  queue->handle();
  queue->handle();
  LOG_ERROR(logger) << "Aegir failed to control thrust\n";
  queue->handle();
  queue->handle();
  std::this_thread::sleep_for(std::chrono::seconds(4));
}

} // namespace helios::logging
