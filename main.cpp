// src/main.cpp
#include "async.h"
#include <exception>
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char *argv[]) {
  // if (argc != 2) {
  //   std::cerr << "Usage: " << argv[0] << " N" << std::endl;
  //   return 1;
  // }
  std::size_t N = 3;
  try {
    auto ptr =
        std::unique_ptr<async::context_type, void (*)(async::context_type *)>(
            new async::context_type(async::connect(N)),
            [](async::context_type *p) {
              async::disconnect(*p);
              delete p;
            });

    std::string command{};
    while (std::getline(std::cin, command)) {
      async::receive(*ptr, command.data(), command.size());
    }
  } catch (const std::exception &err) {
    std::cout << "error : " << err.what() << "\n";
  }
  return 0;
}

// try {
//   N = std::stoi(argv[1]);
// } catch (const std::exception &e) {
//   std::cerr << "Usage: " << argv[0] << " N" << std::endl;
// };
// auto pub = std::make_shared<PublisherValue>();
// auto log_queue = std::make_shared<BlockingQueueValue>();
// auto file_queue = std::make_shared<BlockingQueueValue>();
// auto sub = std::make_shared<Sub>(log_queue, file_queue);
// pub->subscribe(sub);
// std::thread logThread(&log, log_queue);
// std::thread file1Thread(&file, file_queue, "file1");
// std::thread file2Thread(&file, file_queue, "file2");
// try {
//   Status s(N, pub);
//   s.run();
// } catch (const std::exception &e) {
//   std::cerr << "Error: " << e.what() << std::endl;
// };
// log_queue.get()->add({});  // EXIT FROM LOG
// file_queue.get()->add({}); // EXIT FROM FILE
// file_queue.get()->add({}); // EXIT FROM FILE
// logThread.join();
// file1Thread.join();
// file2Thread.join();

// #include "async.h"
// #include "blocking_queue.h"
// #include "status.h"
// #include <fstream>
// #include <iostream>
// #include <thread>

// using BlockingQueueValue = BlockingQueue<Value>;

// // получение пачек
// struct Sub : public pubsub::Subscriber<Value> {
//   std::weak_ptr<BlockingQueueValue> log;
//   std::weak_ptr<BlockingQueueValue> file;
//   Sub(std::weak_ptr<BlockingQueueValue> log,
//       std::weak_ptr<BlockingQueueValue> file)
//       : log(log), file(file){};
//   void callback(value_type message) override {
//     if (auto ptr = log.lock()) {
//       auto message1 = message; // COPY
//       ptr->add(message1);
//     }
//     if (auto ptr = file.lock()) {
//       ptr->add(message);
//     }
//   }
// };

// // вывод на экран
// void log(std::weak_ptr<BlockingQueueValue> queue) {
//   while (true) {
//     if (auto ptr = queue.lock()) {
//       auto val = ptr->take(); // WAIT
//       if (val.done()) {
//         return;
//       }
//       std::cout << "bulk: ";
//       auto vec = val.vector();
//       auto it = vec.begin();
//       while (it != vec.end()) {
//         std::cout << *it;
//         if (++it != vec.end()) {
//           std::cout << ", ";
//         }
//       }
//       std::cout << std::endl;
//     }
//   }
// };

// // вывод в файл
// void file(std::weak_ptr<BlockingQueueValue> queue, std::string prefix) {
//   while (true) {
//     if (auto ptr = queue.lock()) {
//       auto val = ptr->take(); // WAIT
//       if (val.done()) {
//         return;
//       }
//       auto ts = val.time_stamp();
//       std::ofstream file("bulk" + ts.String() + +"." + prefix + ".log");
//       if (file.is_open()) {
//         file << "bulk: ";
//         auto vec = val.vector();
//         auto it = vec.begin();
//         while (it != vec.end()) {
//           file << *it;
//           if (++it != vec.end()) {
//             file << ", ";
//           }
//         }
//         file << std::endl;
//       }
//     } else {
//       return;
//     }
//   }
// }

// int main(int argc, char *argv[]) {
//   // if (argc != 2) {
//   //   std::cerr << "Usage: " << argv[0] << " N" << std::endl;
//   //   return 1;
//   // }
//   std::size_t N = 3;
//   // try {
//   //   N = std::stoi(argv[1]);
//   // } catch (const std::exception &e) {
//   //   std::cerr << "Usage: " << argv[0] << " N" << std::endl;
//   // };
//   auto pub = std::make_shared<PublisherValue>();
//   auto log_queue = std::make_shared<BlockingQueueValue>();
//   auto file_queue = std::make_shared<BlockingQueueValue>();
//   auto sub = std::make_shared<Sub>(log_queue, file_queue);
//   pub->subscribe(sub);
//   std::thread logThread(&log, log_queue);
//   std::thread file1Thread(&file, file_queue, "file1");
//   std::thread file2Thread(&file, file_queue, "file2");
//   try {
//     Status s(N, pub);
//     s.run();
//   } catch (const std::exception &e) {
//     std::cerr << "Error: " << e.what() << std::endl;
//   };
//   log_queue.get()->add({});  // EXIT FROM LOG
//   file_queue.get()->add({}); // EXIT FROM FILE
//   file_queue.get()->add({}); // EXIT FROM FILE
//   logThread.join();
//   file1Thread.join();
//   file2Thread.join();
//   return 0;
// }

// const int num_threads = 3;

// class ContextImpl final : public Context,
//                           public std::enable_shared_from_this<ContextImpl> {
// private:
//   id_type m_id{0};
//   size_type N{3};
//   std::string m_command{};
//   bool string_is_ready{false};

//   std::vector<std::thread> workers;
//   std::mutex m;
//   std::condition_variable cv;

// public:
//   ContextImpl(id_type id, size_type N) : m_id(id), N(N) {}

//   ~ContextImpl() { stop(); }

//   void start() noexcept {
//     for (size_t i = 0; i < num_threads; ++i) {
//       workers.emplace_back(
//           [self = shared_from_this(), i] { self->getline(i); });
//     }
//   }

//   void stop() noexcept {
//     for (auto &v : workers) {
//       if (v.joinable()) {
//         v.join();
//       }
//     }
//   }

//   id_type id() override { return m_id; };

//   // synthetic thread function
//   void getline(int i) noexcept {
//     while (true) {
//       std::unique_lock lk(m);
//       cv.wait(lk, [this] { return !string_is_ready; });
//       try {
//         std::getline(std::cin, m_command);
//       } catch (const std::exception &err) {
//         std::cerr << "getline error: " << err.what() << std::endl;
//       }
//       string_is_ready = true;
//       lk.unlock();
//       cv.notify_all();
//     }
//   }

//   std::string receive() {
//     std::string command{};
//     std::unique_lock lk(m);
//     cv.wait(lk, [this] { return string_is_ready; });
//     command = std::move(m_command);
//     string_is_ready = false;
//     lk.unlock();
//     cv.notify_all();
//     return command;
//   };

//   void disconnect() { stop(); };
// };

// std::shared_ptr<Context> connect(size_type N) {
//   static size_type counter = 0;
//   ++counter;
//   auto ptr = std::make_shared<ContextImpl>(counter, N);
//   ptr->start();
//   return ptr;
// };
