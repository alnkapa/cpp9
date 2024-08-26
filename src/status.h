#ifndef STATUS_H
#define STATUS_H
#include "async.h"
#include "blocking_queue.h"
#include "pub_sub.h"
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
  std::string String() { return std::to_string(m_time_stamp); };
};

class Value {
private:
  enum Status {
    DATA,
    DONE,
  };
  Status m_status;
  TimeStamp m_time_stamp;
  std::vector<std::string> m_vector;

public:
  Value(TimeStamp time_stamp, std::vector<std::string> &&vector)
      : m_status(DATA), m_time_stamp(time_stamp), m_vector(std::move(vector)){};
  Value() : m_status(DONE){};
  const TimeStamp &time_stamp() { return m_time_stamp; };
  const std::vector<std::string> &vector() { return m_vector; };
  bool done() { return m_status == DONE; }
};

const std::string OPEN{"{"};
const std::string CLOSE{"}"};

// Обертка для чтения из библиотеки
struct InputWrapper {
  std::shared_ptr<async::Context> m_ctx;
  InputWrapper(std::size_t N) { m_ctx = async::connect(N); };
  ~InputWrapper() { async::disconnect(m_ctx); };
};

using PublisherValue = pubsub::Publisher<Value>;

class StatusBlockPlus : private InputWrapper {
private:
  std::size_t N{3};
  std::size_t counter{0};
  std::vector<std::string> m_store{};

public:
  StatusBlockPlus(std::size_t N) : N(N), InputWrapper(N){};
  ~StatusBlockPlus(){};
  std::vector<std::string> run();
};

class StatusBlock : private InputWrapper {
private:
  std::size_t N{3};
  std::size_t counter{0};
  TimeStamp m_time_stamp{0};
  std::vector<std::string> m_store{};
  std::weak_ptr<PublisherValue> m_pub;
  void print();

public:
  StatusBlock(std::size_t N, std::weak_ptr<PublisherValue> pub)
      : N(N), m_pub(pub), InputWrapper(N){};
  ~StatusBlock(){};
  void run();
};

class Status : private InputWrapper {
private:
  std::size_t N{3};
  TimeStamp m_time_stamp{0};
  std::vector<std::string> m_store{};
  std::weak_ptr<PublisherValue> m_pub;
  void print();

public:
  Status(std::size_t N, std::weak_ptr<PublisherValue> pub)
      : N(N), m_pub(pub), InputWrapper(N){};
  ~Status() { print(); };
  void run();
};
#endif