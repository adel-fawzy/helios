#include "long_running_thread.hpp"

#include <future>

namespace helios::core
{
  LongRunningThread::LongRunningThread(WorkFunc w, PredicateFunc p) : w_{w}, p_{p}
  {
    std::promise<void> started;
    auto loop = [this, &started]
    {
      started.set_value(); // Indicate that the function has started
      std::unique_lock<std::mutex> lock(mtx_);
      while (true)
      {
        cv_.wait(lock, [this]
                 { return p_() || stop_; }); // Sleep until there is work to do or to stop
        if (stop_)
        {
          // Stop is issued
          break; // Exit thread
        }
        // Release the lock while running the client's work
        lock.unlock();
        try
        {
          w_();
        }
        catch (const std::exception &e)
        {
          // TODO: Log e.what()
        }
        catch (...)
        {
          // TODO: Log an error
        }
        lock.lock();
      }
    };
    t_ = std::thread(loop); // Start thread
    // Wait until the thread is started
    auto fut = started.get_future();
    fut.wait();
  }

  LongRunningThread::~LongRunningThread()
  {
    {
      std::lock_guard<std::mutex> lock(mtx_);
      stop_ = true; // Indicate that the thread should be stopped
    }
    notify();  // Wake the thread in case it is sleeping
    t_.join(); // Wait for the thread to exit
  }

  void LongRunningThread::notify()
  {
    cv_.notify_one();
  }
}; // namespace helios::core
