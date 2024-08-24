// src/main.cpp
#include "async.h"
#include "blocking_queue.h"
#include "status.h"
#include <iostream>
#include <thread>

void log() {}
void file1() {}
void file2() {}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " N" << std::endl;
    return 1;
  }
  int N = std::stoi(argv[1]);
  //Status read(N, Status::BLOCK::OFF);

  // std::thread logThread(log);
  // std::thread file1Thread(file1);
  // std::thread file2Thread(file2);
  // logThread.join();
  // file1Thread.join();
  // file2Thread.join();

  //read.reader();
  return 0;
}
