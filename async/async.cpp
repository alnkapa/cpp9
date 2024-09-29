// library/async.cpp
#include "async.h"
#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

#include <condition_variable>
#include <thread>
#include <vector>

namespace async {

const int num_threads = 3;

class ContextImpl final : public Context,
                          public std::enable_shared_from_this<ContextImpl> {
private:
  id_type m_id{0};
  size_type N{3};
  std::string m_command{};
  bool string_is_ready{false};

  std::vector<std::thread> workers;
  std::mutex m;
  std::condition_variable cv;

public:
  ContextImpl(id_type id, size_type N) : m_id(id), N(N) {}

  ~ContextImpl() { stop(); }

  void start() noexcept {
    for (size_t i = 0; i < num_threads; ++i) {
      workers.emplace_back(
          [self = shared_from_this(), i] { self->getline(i); });
    }
  }

  void stop() noexcept {
    for (auto &v : workers) {
      if (v.joinable()) {
        v.join();
      }
    }
  }

  id_type id() override { return m_id; };

  // synthetic thread function
  void getline(int i) noexcept {
    while (true) {
      std::unique_lock lk(m);
      cv.wait(lk, [this] { return !string_is_ready; });
      try {
        std::getline(std::cin, m_command);
      } catch (const std::exception &err) {
        std::cerr << "getline error: " << err.what() << std::endl;
      }
      string_is_ready = true;
      lk.unlock();
      cv.notify_all();
    }
  }

  std::string receive() {
    std::string command{};
    std::unique_lock lk(m);
    cv.wait(lk, [this] { return string_is_ready; });
    command = std::move(m_command);
    string_is_ready = false;
    lk.unlock();
    cv.notify_all();
    return command;
  };

  void disconnect() { stop(); };
};

std::shared_ptr<Context> connect(size_type N) {
  static size_type counter = 0;
  ++counter;
  auto ptr = std::make_shared<ContextImpl>(counter, N);
  ptr->start();
  return ptr;
};

std::unique_ptr<std::string> receive(std::weak_ptr<Context> ctx_ptr) {
  if (auto ptr = ctx_ptr.lock()) {
    if (auto ptr1 = std::dynamic_pointer_cast<ContextImpl>(ptr)) {
      return std::make_unique<std::string>(ptr1->receive());
    } else {
      throw std::runtime_error("dynamic_pointer_cast error");
    }
  } else {
    throw std::runtime_error("weak_ptr lock error");
  }
};

void disconnect(std::weak_ptr<Context> ctx_ptr) {
  if (auto ptr = ctx_ptr.lock()) {
    if (auto ptr1 = std::dynamic_pointer_cast<ContextImpl>(ptr)) {
      ptr1->disconnect();
    };
  };
};

} // namespace async
