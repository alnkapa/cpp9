#ifndef STATUS_H
#define STATUS_H
#include "blocking_queue.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <tuple>
#include <vector>

using unix_time_stamp_t = long long;
using print_t = std::pair<unix_time_stamp_t, std::vector<std::string>>;

template <std::size_t N> class StatusSwitcher {
  long long m_time_stamp{0};
  std::string m_command{};
  static std::vector<std::string> store{};
public:
  StatusSwitcher(){};
  void run() {
    while (std::getline(std::cin, m_command)) {
      if (m_command.empty()) {
        break;
      }
      if (m_command == "{") {
        m_time_stamp = 0;        
      } else if (m_command == "}") {        
      } else {
        if (m_time_stamp == 0) {
          m_time_stamp =
              std::chrono::duration_cast<std::chrono::seconds>(
                  std::chrono::system_clock::now().time_since_epoch())
                  .count();
        }
        store.emplace_back(std::move(m_command));        
      }
    }
  }
};

template <std::size_t N> class StatusSwitcherBlock : public StatusSwitcher<N> {
  void status() {
    static std::vector<std::string> store{};
    store.emplace_back(std::move(m_command));
    if (store.size() > N) {
      // TODO: print
      ;
      ;
      store.clear();
    }
  };
  void statusBlock() { static std::vector<std::string> store{}; };
  void statusBlockPlus(){};
  long long m_time_stamp{0};
  int m_block_counter{0};
  std::string m_command;
  void (StatusSwitcher::*m_status)() = &StatusSwitcher::status;

public:
  StatusSwitcher(){};
  void run() {
    while (std::getline(std::cin, m_command)) {
      if (m_command.empty()) {
        break;
      }
      if (m_command == "{") {
        m_time_stamp = 0;
        if (m_block_counter == 0) {
          m_status = &StatusSwitcher::statusBlock;
        } else {
          m_status = &StatusSwitcher::statusBlockPlus;
        }
        m_block_counter++;
      } else if (m_command == "}" && m_block_counter > 0) {
        m_block_counter--;
        if (m_block_counter == 1) {
          m_status = &StatusSwitcher::statusBlock;
        } else {
          m_status = &StatusSwitcher::statusBlockPlus;
        }
      } else {
        if (m_time_stamp == 0) {
          m_time_stamp =
              std::chrono::duration_cast<std::chrono::seconds>(
                  std::chrono::system_clock::now().time_since_epoch())
                  .count();
        }
        (this->*m_status)();
      }
    }
  }
};

// class Status {
// public:
//   enum class BLOCK : int { OFF = 0, ON = 1, INNER = 2 };
//   // using Publisher<print_t>::subscribe;
//   // using Publisher<print_t>::unsubscribe;

// private:
//   std::vector<std::string> m_stack;
//   long long m_time_stamp{0};
//   std::weak_ptr<BlockingQueue<print_t>> m_queue_ptr;
//   int m_n{3};
//   int m_counter{0};
//   BLOCK m_block_hierarchy{BLOCK::OFF};
//   bool m_block_end{false};
//   void print() {
//     if (m_stack.size() > 0) {
//       if (auto ptr = m_queue_ptr.lock()) {
//         ptr->add({m_time_stamp, m_stack});
//       }
//     }
//   };

//   void clear() {
//     m_stack.clear();
//     m_counter = 0;
//     m_time_stamp = 0;
//   };

// public:
//   Status(int N, BLOCK block_status,
//          std::weak_ptr<BlockingQueue<print_t>> queue_ptr)
//       : m_n(N), m_block_hierarchy(block_status), m_queue_ptr(queue_ptr){};
//   ~Status() {
//     if (m_block_hierarchy == BLOCK::OFF) {
//       print();
//     }
//     clear();
//   };

//   void reader() {
//     std::string command;
//     while (std::getline(std::cin, command)) {
//       if (command.empty()) {
//         break;
//       }
//       if (command == "{") {
//         if (m_block_hierarchy == BLOCK::OFF) {
//           print();
//           clear();
//         }
//         // new block
//         auto block_hierarchy = m_block_hierarchy;
//         if (m_block_hierarchy < BLOCK::INNER) {
//           block_hierarchy =
//               static_cast<BLOCK>(static_cast<int>(m_block_hierarchy) + 1);
//         }
//         Status inner_reader(m_n, block_hierarchy);
//         inner_reader.reader();
//         if (m_block_hierarchy > BLOCK::OFF) {
//           for (auto &&v : inner_reader.m_stack) {
//             m_stack.emplace_back(std::move(v));
//           }
//         } else if (inner_reader.m_block_end) {
//           inner_reader.print();
//         }
//         // call dtor
//       } else if (command == "}") {
//         m_block_end = true;
//         // close block
//         if (m_block_hierarchy != BLOCK::OFF) {
//           return;
//         }
//       } else {
//         if (m_time_stamp == 0) {
//           m_time_stamp =
//               std::chrono::duration_cast<std::chrono::seconds>(
//                   std::chrono::system_clock::now().time_since_epoch())
//                   .count();
//         }
//         // in block
//         m_stack.emplace_back(std::move(command));
//         m_counter++;
//         if (m_block_hierarchy == BLOCK::OFF && m_counter == m_n) {
//           print();
//           clear();
//         } else if (m_block_hierarchy > BLOCK::OFF && m_counter > m_n) {
//           clear();
//         }
//       }
//     }
//     return;
//   };
// };

#endif