// library/async.cpp
#include "async.h"
#include "blocking_queue.h"
#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>

namespace async
{

class Worker
{
  private:
    struct Done
    {
    };
    std::thread m_thread;
    size_type n;
    std::atomic<bool> m_running{false};
    BlockingQueue<std::variant<Done, std::string>> m_queue;

  public:
    Worker(size_type N)
        : n(N) {}

    ~Worker()
    {
        stop();
    }

    void
    run()
    {
        if (!m_running.exchange(true)) // set true if false
        {
            m_thread = std::thread(&Worker::proccess, this);
        }
    }

    void
    stop()
    {
        if (m_running.exchange(false)) // set false if true
        {
            m_queue.add(Done{});
        }

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void
    receive(const char_type *ptr, size_type size)
    {
        if (m_running)
        {
            m_queue.add(std::string{ptr, size});
        }
    }

    void
    proccess()
    {
        while (m_running)
        {
            auto val = m_queue.take(); // block here
            if (std::holds_alternative<std::string>(val))
            {
                std::string message = std::get<std::string>(val);
                std::cout << "message" << message << "\n";
            }
            else
            {
                break;
            }
        }
        m_running = false;
    }
};

class Publisher
{
  private:
    std::unordered_map<context_type, std::shared_ptr<Worker>> m_subscribers;
    std::mutex lo;

  public:
    void
    subscribe(context_type &ctx, size_type N)
    {
        auto w = std::make_shared<Worker>(N);
        {
            std::lock_guard lock(lo);
            m_subscribers.emplace(ctx, w);
        }
        w->run();
    }

    void
    unsubscribe(context_type &ctx)
    {
        std::lock_guard lock(lo);
        auto it = m_subscribers.find(ctx);
        if (it == m_subscribers.end())
        {
            throw std::runtime_error("Context not found");
        }
        m_subscribers.erase(it);
    }

    void
    notify(context_type &ctx, const char_type *ptr, size_type size)
    {
        std::lock_guard lock(lo);
        auto it = m_subscribers.find(ctx);
        if (it == m_subscribers.end())
        {
            throw std::runtime_error("Context not found");
        }
        it->second->receive(ptr, size);
    }
};

static Publisher pub{};
std::atomic<size_type> counter{0};

context_type
connect(size_type N)
{
    context_type ctx = ++counter;
    pub.subscribe(ctx, N);
    return ctx;
}

void
receive(context_type &ctx, const char_type *ptr, size_type size)
{
    pub.notify(ctx, ptr, size);
}

void
disconnect(context_type &ctx)
{
    pub.unsubscribe(ctx);
}

} // namespace async
