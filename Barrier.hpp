#pragma ones


#include <condition_variable>
#include <cstddef>
#include <mutex>

class CyclicBarrier {
 public:
  explicit CyclicBarrier(const size_t num_threads)
      : num_threads_(num_threads),
        arrived_thread_count_(0),
        phase_(true) {}

  bool PassThrough() {
    std::unique_lock<std::mutex> lock{mutex_};
    bool current_phase = phase_;
    ++arrived_thread_count_;
    if (arrived_thread_count_ == num_threads_) {
      phase_ = !current_phase;
      arrived_thread_count_ = 0;
      phase_changed_.notify_all();
      // последний прошедший поток
      return true;
    } else {
      phase_changed_.wait(lock, [this, &current_phase]() { return phase_ == !current_phase; });
    }
    return false;
  }

 private:
  std::mutex mutex_;
  std::condition_variable phase_changed_;
  size_t num_threads_;
  size_t arrived_thread_count_;
  bool phase_;
};
