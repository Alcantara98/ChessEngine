#include "thread_handler.h"

namespace engine::parts
{
// CONSTRUCTORS

ThreadHandler::ThreadHandler(std::atomic<bool> &running_flag,
                             std::function<void()> function)
    : running_flag(running_flag), function(std::move(function))

{
}

// DESTRUCTOR

ThreadHandler::~ThreadHandler() { stop_thread(); }

// PUBLIC FUNCTIONS

void ThreadHandler::start_thread(int thread_timeout_ms)
{
  running_flag = true;
  worker_thread = std::thread(
      [this, thread_timeout_ms]()
      {
        function();
        // Notify the timeout thread that the search is complete.
        running_flag = false;
        std::lock_guard<std::mutex> lock(search_timeout_mutex);
        search_timeout_cv.notify_one();
      });

  timeout_thread = std::thread([this, thread_timeout_ms]()
                               { initiate_search_timeout(thread_timeout_ms); });
}

void ThreadHandler::stop_thread()
{
  running_flag = false;
  if (worker_thread.joinable())
  {
    worker_thread.join();
  }

  if (timeout_thread.joinable())
  {
    timeout_thread.join();
  }
}

// PRIVATE FUNCTIONS

void ThreadHandler::initiate_search_timeout(const int thread_timeout_ms)
{
  std::unique_lock<std::mutex> lock(search_timeout_mutex);
  if (search_timeout_cv.wait_for(
          lock, std::chrono::milliseconds(thread_timeout_ms)) ==
      std::cv_status::timeout)
  {
    running_flag = false;
  }
}
} // namespace engine::parts