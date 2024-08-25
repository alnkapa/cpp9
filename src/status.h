#ifndef STATUS_H
#define STATUS_H
#include "async.h"
#include "blocking_queue.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <tuple>
#include <variant>
#include <vector>

class TimeStamp {
public:
  using value_type = long long;

private:
  value_type m_time_stamp{0};

public:
  TimeStamp(value_type in = 0) : m_time_stamp(in){};
  void reset() { m_time_stamp = 0; };
  void update() {
    if (m_time_stamp == 0) {
      m_time_stamp = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count();
    }
  }
  operator value_type() { return m_time_stamp; };
};

class Value {
public:
  enum class Status { DATA, DONE };

private:
  std::vector<std::string> m_vec;
  Status m_status;
  TimeStamp::value_type m_time_stamp{};

public:
  Value(TimeStamp::value_type t_stamp, std::vector<std::string> &&vec)
      : m_vec(std::move(vec)) {
    m_status = Status::DATA;
  };
  Value() { m_status = Status::DONE; };
  Status status() { return m_status; };
  bool data() { return m_status == Status::DATA; };
  bool done() { return m_status == Status::DONE; };
  std::vector<std::string> vector() { return m_vec; };
  TimeStamp::value_type time_stamp() { return m_time_stamp; };
};

const std::string OPEN{"{"};
const std::string CLOSE{"}"};

class StatusBlockPlus {
private:
  std::size_t N{3};
  std::size_t counter{0};
  std::vector<std::string> m_store{};

public:
  StatusBlockPlus(std::size_t N) : N(N){};
  ~StatusBlockPlus(){};
  std::vector<std::string> run();
};

class StatusBlock {
private:
  std::size_t N{3};
  std::size_t counter{0};
  TimeStamp m_time_stamp{0};
  std::vector<std::string> m_store{};
  std::weak_ptr<BlockingQueue<Value>> m_queue;
  std::unique_ptr<StatusBlockPlus> m_block;
  void print() {
    if (!m_store.empty()) {
      if (auto ptr = m_queue.lock()) {
        ptr->add({m_time_stamp, std::move(m_store)});
      }
    }
  };

public:
  StatusBlock(std::size_t N, std::weak_ptr<BlockingQueue<Value>> b)
      : N(N), m_queue(b) {
    m_block = std::make_unique<StatusBlockPlus>(N);
  };
  ~StatusBlock(){};
  void run();
};

class Status {

private:
  std::size_t N{3};
  TimeStamp m_time_stamp{0};
  std::vector<std::string> m_store{};
  std::weak_ptr<BlockingQueue<Value>> m_queue;
  std::unique_ptr<StatusBlock> m_block;
  std::shared_ptr<async::Context> m_ctx;
  void print() {
    if (!m_store.empty()) {
      if (auto ptr = m_queue.lock()) {
        ptr->add({m_time_stamp, std::move(m_store)});
      }
    }
  };

public:
  Status(std::size_t N, std::weak_ptr<BlockingQueue<Value>> b)
      : N(N), m_queue(b) {
    m_block = std::make_unique<StatusBlock>(N, b);
    m_ctx = async::connect(N);
  };
  ~Status() {
    print();
    async::disconnect(m_ctx);
  };
  void run();
};
#endif