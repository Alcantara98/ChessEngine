#ifndef THREAD_HANDLER_H
#define THREAD_HANDLER_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <thread>

namespace engine::parts
{
class ThreadHandler
{
public:
  // CONSTRUCTORS
  /**
   * @brief Constructor to create a thread handler.
   *
   * @param stop_flag Atomic flag to stop the thread.
   * @param function Function to run in the thread.
   * @param args Arguments to pass to the function.
   */
  ThreadHandler(std::atomic<bool> &running_flag,
                std::function<void()> function);

  // DESTRUCTOR
  /**
   * @brief Destructor to join the thread.
   */
  ~ThreadHandler();

  // FUNCTIONS
  /**
   * @brief Starts the thread.
   *
   * @param thread_timeout Timeout for the thread.
   */
  void start_thread(int thread_timeout);

  /**
   * @brief Stops the thread.
   */
  void stop_thread();

private:
  // PROPERTIES

  /// @brief Atomic flag to stop the thread. If false, the thread will stop.
  std::atomic<bool> &running_flag;

  /// @brief Thread object.
  std::thread worker_thread;

  /// @brief For stopping the worker thread.
  std::thread timeout_thread;

  /// @brief For stopping the thread.
  std::condition_variable search_timeout_cv;
  std::mutex search_timeout_mutex;

  /// @brief Function to run in the thread.
  std::function<void()> function;

  // FUNCTIONS
  /**
   * @brief Initiates the search timeout.
   *
   * @details The function will wait for the timeout to occur and then set the
   * stop flag to true.
   */
  void initiate_search_timeout(int thread_timeout);
};
} // namespace engine::parts
#endif // THREAD_HANDLER_H