#ifndef THREAD_HANDLER_H
#define THREAD_HANDLER_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <thread>

namespace engine::parts
{
/**
 * @brief Class to run a function in a separate thread.
 */
class ThreadHandler
{
public:
  // CONSTRUCTORS
  /**
   * @brief Constructor to create a thread handler.
   *
   * @param running_flag Reference to an atomic flag. If set to false, the thread will stop.
   * @param function Function to run in the thread.
   */
  ThreadHandler(std::atomic<bool> &running_flag,
                std::function<void()> function);

  // DESTRUCTOR
  /**
   * @brief Destructor to join the thread.
   *
   * @details Ensures that the worker thread is joined before the object is destroyed.
   */
  ~ThreadHandler();

  // FUNCTIONS
  /**
   * @brief Starts the thread.
   *
   * @param thread_timeout_ms Timeout for the thread in milliseconds.
   *
   * @details This function starts the worker thread and a timeout thread. The timeout thread
   * will stop the worker thread after the specified timeout.
   */
  void start_thread(int thread_timeout_ms);

  /**
   * @brief Stops the thread.
   *
   * @details Sets the running flag to false and notifies the condition variable to stop the thread.
   */
  void stop_thread();

private:
  // PROPERTIES

  /// @brief Atomic flag to stop the thread. When set to false, the thread will stop.
  std::atomic<bool> &running_flag;

  /// @brief Thread object to run the given function.
  std::thread worker_thread;

  /// @brief Thread object to handle the timeout for the worker thread.
  std::thread timeout_thread;

  /// @brief Condition variable for managing thread timeout.
  std::condition_variable search_timeout_cv;

  /// @brief Mutex for synchronizing thread timeout operations.
  std::mutex search_timeout_mutex;

  /// @brief Function to run in the thread.
  std::function<void()> function;

  // FUNCTIONS
  /**
   * @brief Initiates the search timeout.
   *
   * @details The function waits for the specified timeout duration and then sets the
   * running flag to false, signaling the worker thread to stop.
   *
   * @param thread_timeout_ms Timeout for the thread in milliseconds.
   */
  void initiate_search_timeout(int thread_timeout_ms);
};
} // namespace engine::parts
#endif // THREAD_HANDLER_H