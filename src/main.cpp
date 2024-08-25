// src/main.cpp
#include "async.h"
#include "blocking_queue.h"
#include "status.h"
#include <iostream>
#include <thread>

// вывод на экран
void log(std::weak_ptr<BlockingQueue<Value>> in_q,
         std::weak_ptr<BlockingQueue<Value>> out_q) {
  if (auto ptr_in = in_q.lock()) {
    auto vec = ptr_in->take().vector();
    if (!vec.empty()) {
      std::cout << "bulk: ";
      auto it = vec.begin();
      while (it != vec.end()) {
        std::cout << *it;
        if (++it != vec.end()) {
          std::cout << ", ";
        }
      }
      std::cout << std::endl;
    }
    if (auto ptr = out_q.lock()) {
      // ptr->add(vec);
    }
  }
};
void file1() {}
void file2() {}

int main(int argc, char *argv[]) {
  // if (argc != 2) {
  //   std::cerr << "Usage: " << argv[0] << " N" << std::endl;
  //   return 1;
  // }
  std::size_t N = 3;
  // try {
  //   N = std::stoi(argv[1]);
  // } catch (const std::exception &e) {
  //   std::cerr << "Usage: " << argv[0] << " N" << std::endl;
  // };
  auto pipe = std::make_shared<BlockingQueue<Value>>();
  auto out = std::make_shared<BlockingQueue<Value>>();
  std::thread logThread(&log, pipe, out);
  try {
    Status(N, pipe).run();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  };
  logThread.join();
  // std::thread file1Thread(file1);
  // std::thread file2Thread(file2);
  // file1Thread.join();
  // file2Thread.join();
  return 0;
}
