#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include "GameOfLife.hpp"

int main() {
  GameOfLife* gol = nullptr;
  size_t probability = 40;
  while (true) {
    std::string buf;
    std::getline(std::cin, buf);
    auto bstream = std::stringstream(buf);
    std::string command;
    bstream >> command;
    if (command == "START") {
      if (gol != nullptr) {
        std::cout << "The game has already started";
      }
      else {
        size_t num_threads = 0;
        // Первый символ после START
        if (!std::isdigit(buf[6])) {
          std::string filename;
          bstream >> filename >> num_threads;
          gol = new GameOfLife(filename, num_threads);
        }
        else {
          size_t n, m;
          bstream >> n >> m >> num_threads;
          gol = new GameOfLife(n, m, num_threads, probability);
        }
        gol->Start();
        std::cout << "Game started with field: \n";
        gol->PrintField();
      }
    }
    else if (command == "QUIT") {
      if (gol) {
        gol->Quit();
      }
      break;
    }
    else if (gol == nullptr) {
      std::cout << "Start the game!\n";
    }
    else if (command == "STATUS") {
      gol->Status();
    }
    else if (command == "RUN") {
      size_t num = 0;
      bstream >> num;
      gol->Run(num);
    }
    else if (command == "STOP") {
      gol->Stop();
    }

    else {
      std::cout << "Invalid command\n";
    }
  }
  delete gol;
  return 0;
}
