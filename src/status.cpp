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
  while (true) {
    auto command_ptr = async::receive(m_ctx);
    if (command_ptr->empty()) {
      break;
    }
    if (*command_ptr == OPEN) {
      auto ptr = std::make_unique<StatusBlockPlus>(N, m_ctx);
      auto vec = ptr->run();
      for (auto &&v : vec) {
        m_store.emplace_back(v);
      };
    } else if (*command_ptr == CLOSE) {
      break;
    } else {
      m_store.emplace_back(std::move(*command_ptr));
      counter++;
      if (counter > N) {
        m_store.clear();
        // counter = 0;
      };
    }
  }
  return m_store;
}

void StatusBlock::print() {
  if (!m_store.empty()) {
    if (auto ptr = m_pub.lock()) {
      ptr->notify({m_time_stamp, std::move(m_store)});
    }
  }
}

void StatusBlock::run() {
  while (true) {
    auto command_ptr = async::receive(m_ctx);
    if (command_ptr->empty()) {
      break;
    }
    if (*command_ptr == OPEN) {
      m_time_stamp.reset();
      for (auto &&v : StatusBlockPlus(N, m_ctx).run()) {
        m_store.emplace_back(v);
      }
    } else if (*command_ptr == CLOSE) {
      print();
      break;
    } else {
      m_time_stamp.update();
      m_store.emplace_back(std::move(*command_ptr));
      counter++;
      if (counter > N) {
        m_store.clear();
        // counter = 0;
      };
    }
  }
}

void Status::print() {
  if (!m_store.empty()) {
    if (auto ptr = m_pub.lock()) {
      ptr->notify({m_time_stamp, std::move(m_store)});
    }
  }
}

void Status::run() {
  while (true) {
    auto command_ptr = async::receive(m_ctx);
    if (command_ptr->empty()) {
      break;
    }
    if (*command_ptr == OPEN) {
      print();
      m_store.clear();
      m_time_stamp.reset();
      StatusBlock(N, m_pub, m_ctx).run();
    } else if (*command_ptr != CLOSE) {
      m_time_stamp.update();
      m_store.emplace_back(std::move(*command_ptr));
      if (m_store.size() >= N) {
        print();
        m_store.clear();
      }
    }
  }
}
