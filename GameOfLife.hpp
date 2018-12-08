#pragma ones

#include "Barrier.hpp"

#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include <thread>

class GameOfLife {
 public:
  GameOfLife(size_t n, size_t m, size_t num_threads, size_t probability = 40) :
      iter_todo_(0),
      iter_done_(0),
      num_threads_(num_threads),
      quit_(false),
      stopped_(true),
      threads_(0),
      rows_num_(n),
      columns_num_(m) {
    old_field_ = field_ = GenerateRandom(n, m, probability);
    CheckNum(n);
    barrier_ = new CyclicBarrier(num_threads_);
  }

  GameOfLife(const std::string &filename, size_t num_threads) :
      iter_todo_(0),
      iter_done_(0),
      num_threads_(num_threads),
      quit_(false),
      stopped_(true),
      threads_(0) {
    old_field_ = field_ = ReadCSV(filename);
    rows_num_ = field_.size();
    columns_num_ = field_[0].size();
    CheckNum(rows_num_);
    barrier_ = new CyclicBarrier(num_threads_);
  }

  void PrintField();

  void Start();

  void Run(size_t num);

  void Status();

  void Stop();

  void Quit();

 private:
  std::vector<std::vector<short>> GenerateRandom(size_t n, size_t m, size_t probability);

  std::vector<std::vector<short>> ReadCSV(const std::string &filename);

  void ThreadFunction(size_t min_index, size_t max_index);

  void ProcessCell(size_t i, size_t j);

  short GetByInd(size_t i, size_t j);

  void CheckNum(size_t length);




 private:
  std::vector<std::vector<short>> field_;
  std::vector<std::vector<short>> old_field_;
  std::vector<std::thread> threads_;

  std::atomic<size_t> iter_todo_;
  std::atomic<size_t> iter_done_;
  std::atomic<bool> stopped_;
  std::atomic<bool> quit_;

  size_t rows_num_;
  size_t columns_num_;
  size_t num_threads_;
  CyclicBarrier *barrier_;
  std::mutex iter_lock_;
};