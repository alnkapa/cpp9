#ifndef STATUS_H
#define STATUS_H

#include "blocking_queue.h"
#include "pub_sub.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

class TimeStamp
{
  public:
    using value_type = long long;

  private:
    value_type m_time_stamp{0};

  public:
    TimeStamp(value_type in = 0)
        : m_time_stamp(in) {}

    void
    reset()
    {
        m_time_stamp = 0;
    }

    void
    update()
    {
        if (m_time_stamp == 0)
        {
            m_time_stamp = std::chrono::duration_cast<std::chrono::seconds>(
                               std::chrono::system_clock::now().time_since_epoch())
                               .count();
        }
    }

    std::string
    String() const
    {
        return std::to_string(m_time_stamp);
    }
};

class Value
{
  private:
    enum Status
    {
        DATA,
        DONE,
    };

    Status m_status;
    TimeStamp m_time_stamp;
    std::vector<std::string> m_vector;

  public:
    Value(TimeStamp time_stamp, std::vector<std::string> &&vector)
        : m_status(DATA), m_time_stamp(time_stamp), m_vector(std::move(vector)) {}

    Value()
        : m_status(DONE) {}

    const TimeStamp &
    time_stamp() const
    {
        return m_time_stamp;
    }
    const std::vector<std::string> &
    vector() const
    {
        return m_vector;
    }
    bool
    done() const
    {
        return m_status == DONE;
    }
};

const std::string OPEN{"{"};
const std::string CLOSE{"}"};

const int no_block_status = 0;
const int block_status = 1;
const int block_plus_status = 2;

using PublisherValue = pubsub::Publisher<Value>;
using sub_type = pubsub::Publisher<std::vector<std::string>>;

class StatusBlockPlus : public sub_type
{
  private:
    using stack_t = std::pair<std::vector<std::string>, std::size_t>;
    std::size_t N{3};
    std::size_t m_level{0};
    std::vector<std::string> m_store;
    bool m_stop{false};
    std::size_t m_stop_level{0};
    std::size_t m_counter{0};
    std::vector<stack_t> m_stack;    

  public:
    StatusBlockPlus(std::size_t N)
        : N(N) {}

    int
    add(std::string &&);
};

class StatusBlock : public pubsub::Subscriber<std::vector<std::string>>
{
  private:
    std::size_t N{3};
    std::size_t m_counter{0};
    TimeStamp m_time_stamp{0};
    std::vector<std::string> m_store{};
    std::weak_ptr<PublisherValue> m_pub;

    void
    print();

    void
    clear();

  public:
    StatusBlock(std::size_t N, std::weak_ptr<PublisherValue> pub)
        : N(N), m_pub(pub){};
    virtual ~StatusBlock(){};
    int
    add(std::string &&);
    void
        callback(std::vector<std::string>) override;
};

class Status
{
  private:
    std::size_t N{3};
    TimeStamp m_time_stamp{0};
    std::vector<std::string> m_store{};
    std::weak_ptr<PublisherValue> m_pub;

    void
    print();

  public:
    Status(std::size_t N, std::weak_ptr<PublisherValue> pub)
        : N(N), m_pub(std::move(pub)) {}

    ~Status() { print(); }

    int
    add(std::string &&);
};

#endif // STATUS_H
