#ifndef BLOCKING_QUEUE_TEST_H
#define BLOCKING_QUEUE_TEST_H

#include "blocking_queue.h"
#include <thread>
#include <vector>

bool testAddAndTake() {
  BlockingQueue<int> queue(5);

  for (int i = 0; i < 5; ++i) {
    queue.add(i);
  }

  for (int i = 0; i < 5; ++i) {
    int value = queue.take();
    if (value != i) {
      return false;
    }
  }
  return true;
}

bool testOverflow() {
  BlockingQueue<int> queue(2);
  std::vector<std::thread> producers;

  for (int i = 0; i < 5; ++i) {
    producers.emplace_back([&queue, i]() { queue.add(i); });
  }

  for (int i = 0; i < 5; ++i) {
    int value = queue.take();
    if (value != i) {
      return false;
    }
  }

  for (auto &producer : producers) {
    producer.join();
  }

  return true;
}

#endif