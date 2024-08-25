#include "status.h"
#include "async.h"
#include "blocking_queue.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <tuple>
#include <variant>
#include <vector>

std::vector<std::string> StatusBlockPlus::run() {

  std::string command{};
  while (std::getline(std::cin, command)) {
    if (command.empty()) {
      break;
    }
    if (command == OPEN) {
      auto ptr = std::make_unique<StatusBlockPlus>(N);
      auto vec = ptr->run();
      for (auto &&v : vec) {
        m_store.emplace_back(v);
      };
    } else if (command == CLOSE) {
      break;
    } else {
      m_store.emplace_back(std::move(command));
      counter++;
      if (counter > N) {
        m_store.clear();
        counter = 0;
      };
    }
  }
  return m_store;
}

void StatusBlock::run() {
  std::string command{};
  while (std::getline(std::cin, command)) {
    if (command.empty()) {
      break;
    }
    if (command == OPEN) {
      m_time_stamp.reset();
      for (auto &&v : m_block->run()) {
        m_store.emplace_back(v);
      }
    } else if (command == CLOSE) {
      print();
      break;
    } else {
      m_time_stamp.update();
      m_store.emplace_back(std::move(command));
      counter++;
      if (counter > N) {
        m_store.clear();
        counter = 0;
      };
    }
  }
}

void Status::run() {
  const async::char_type *ch = nullptr;
  async::size_type size = 0;
  while (true) {
    async::receive(m_ctx, &ch, &size);
    std::string command{ch, size};
    if (command.empty()) {
      break;
    }
    if (command == OPEN) {
      print();
      m_store.clear();
      m_time_stamp.reset();
      m_block->run();
    } else if (command != CLOSE) {
      m_time_stamp.update();
      m_store.emplace_back(std::move(command));
      if (m_store.size() >= N) {
        print();
        m_store.clear();
      }
    }
  }
}
