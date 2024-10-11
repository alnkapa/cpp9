// library/async.cpp
#include "async.h"
#include "blocking_queue.h"
#include "status.h"
#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <variant>

namespace async
{

class Worker
{
  private:
    struct Done
    {
    };
    error_code m_error{error_ok};
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

    error_code
    receive(const char_type *ptr, size_type size)
    {
        if (m_running)
        {
            m_queue.add(std::string{ptr, size});
        }
        return m_error;
    }

    void
    proccess()
    {
        auto pub = std::make_shared<PublisherValue>();
        Status s(n, pub);

        while (m_running)
        {
            auto val = m_queue.take(); // block here
            if (std::holds_alternative<std::string>(val))
            {
                std::string message = std::get<std::string>(val);
                if (!message.empty())
                {
                    break;
                }
                s.run();
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

    error_code
    unsubscribe(context_type &ctx)
    {
        std::lock_guard lock(lo);
        auto it = m_subscribers.find(ctx);
        if (it == m_subscribers.end())
        {
            return error_context_not_found;
        }
        m_subscribers.erase(it);
        return error_ok;
    }

    error_code
    notify(context_type &ctx, const char_type *ptr, size_type size)
    {
        std::lock_guard lock(lo);
        auto it = m_subscribers.find(ctx);
        if (it == m_subscribers.end())
        {
            return error_context_not_found;
        }
        else
        {
            return it->second->receive(ptr, size);
        }
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

error_code
receive(context_type &ctx, const char_type *ptr, size_type size) noexcept
{
    return pub.notify(ctx, ptr, size);
}

error_code
disconnect(context_type &ctx) noexcept
{
    return pub.unsubscribe(ctx);
}

} // namespace async
