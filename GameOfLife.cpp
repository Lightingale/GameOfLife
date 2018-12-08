#include "GameOfLife.hpp"
#include <fstream>
#include <iostream>
#include <random>


void GameOfLife::PrintField() {
  const std::string black = "\u25A3";
  const std::string white = "\u2B1C";
  for (auto fline: old_field_) {
    for (auto symb: fline) {
      if (symb == 1) {
        std::cout << black;
      } else {
        std::cout << white;
      }
      std::cout << ' ';

    }
    std::cout << std::endl;
  }
}


// поле - в формате 0 и 1
std::vector<std::vector<short>> GameOfLife::ReadCSV(const std::string &filename) {
  std::ifstream file(filename);
  std::string line;
  std::vector<std::vector<short>> field;
  while (std::getline(file, line)) {
    size_t n = (line.size() + 1) / 2;
    std::vector<short> fline(n);
    // каждый второй символ - запятая
    for (size_t i = 0; i < n; ++i) {
      fline[i] = static_cast<short>(line[2 * i] - '0');
    }
    field.push_back(fline);
  }
  return field;
}

// принимает вероятность наличия жизни в клетки от 0 до 100
std::vector<std::vector<short>> GameOfLife::GenerateRandom(size_t n,
                                                           size_t m,
                                                           size_t probability) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> uniform_dis(1, 100);
  std::vector<std::vector<short>> field(n);
  for (auto &fline: field) {
    fline = std::vector<short>(m);
    for (auto &symb: fline) {
      int rand = uniform_dis(gen);
      if (rand <= probability) {
        symb = 1;
      }
    }
  }
  return field;
}

void GameOfLife::CheckNum(size_t length) {
  if (num_threads_ > length) {
    num_threads_ = length;
    std::cout << "The number of threads is reduced to " <<
              length << " (height of the field)\n";
  }

}

void GameOfLife::Start() {
  size_t thread_rows = field_.size() / num_threads_;
  // максимум не включается
  size_t min_index = 0, max_index = 0;
  for (int i = 0; i < num_threads_; ++i) {
    min_index = max_index;
    if (i == num_threads_ - 1) {
      max_index = field_.size();
    } else {
      max_index = min_index + thread_rows;
    }
    threads_.emplace_back([=] { ThreadFunction(min_index, max_index); });
  }
}

void GameOfLife::Run(size_t num) {
  stopped_.store(false);
  if (num > 0) {
    std::lock_guard lock(iter_lock_);
    iter_todo_.store(iter_todo_.load() + num);
  }
}

void GameOfLife::Status() {
  if (stopped_.load()) {
    // дождемся окончания вычислений
    while (iter_done_.load() != iter_todo_.load());
    PrintField();
  } else {
    std::cout << "Stop the game first\n";
  }
}

void GameOfLife::Stop() {
  if(stopped_.load()) {
    return;
  }
  stopped_.store(true);
  std::lock_guard lock(iter_lock_);
  // доделаем текущую итерацию, если она есть
  if (iter_done_.load() < iter_todo_.load()) {
    iter_todo_.store(iter_done_.load() + 1);
  }
  std::cout << "Game stopped, " << iter_todo_.load() << " iterations done.\n";
}

void GameOfLife::Quit() {
  std::cout << "Quitting game..." << '\n';
  Stop();
  while (iter_done_.load() != iter_todo_.load());
  quit_.store(true);
  for (auto &thread: threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  std::cout << "The end." << '\n';
}

void GameOfLife::ThreadFunction(size_t min_index, size_t max_index) {
  int iter_local = 0;
  while (!quit_.load()) {
    if (iter_local == iter_done_.load() && iter_done_.load() < iter_todo_.load()) {
      ++iter_local;

      for (size_t i = min_index; i < max_index; ++i) {
        for (size_t j = 0; j < columns_num_; ++j) {
          ProcessCell(i, j);
        }
      }

      if (barrier_->PassThrough()) {
        std::lock_guard lock(iter_lock_);
        std::swap(field_, old_field_);
        iter_done_.store(iter_done_.load() + 1);
        // для тестов
//        PrintField();
//        std::cout << '\n';
      }
    }
  }
}

void GameOfLife::ProcessCell(size_t i, size_t j) {
  short sum = 0;
  for (int k = -1; k <= 1; ++k) {
    for (int m = -1; m <= 1; ++m) {
      sum += GetByInd(i + k, j + m);
    }
  }
  short curr = old_field_[i][j];
  sum -= curr;
  if (curr == 1) {
    if (sum == 2 || sum == 3) {
      field_[i][j] = 1;
    }
    else {
      field_[i][j] = 0;
    }
  }
  else {
    if (sum == 3) {
      field_[i][j] = 1;
    }
    else {
      field_[i][j] = 0;
    }
  }
}

short GameOfLife::GetByInd(size_t i, size_t j) {
  size_t new_i = (i + rows_num_) % rows_num_;
  size_t new_j = (j + columns_num_) % columns_num_;
  return old_field_[new_i][new_j];
}





