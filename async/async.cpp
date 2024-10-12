// library/async.cpp
#include "async.h"
#include "blocking_queue.h"
#include "pub_sub.h"
#include "status.h"
#include <algorithm>
#include <atomic>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <variant>

namespace async
{

using BlockingQueueValue = BlockingQueue<Value>;

// получение пачек
struct Sub : public pubsub::Subscriber<Value>
{
    std::weak_ptr<BlockingQueueValue> log;
    std::weak_ptr<BlockingQueueValue> file;
    Sub(std::weak_ptr<BlockingQueueValue> log,
        std::weak_ptr<BlockingQueueValue> file)
        : log(log), file(file){};
    void
    callback(value_type message) override
    {
        if (auto ptr = log.lock())
        {
            auto message1 = message; // COPY
            ptr->add(message1);
        }
        if (auto ptr = file.lock())
        {
            ptr->add(message);
        }
    }
};

// вывод на экран
void
log(std::weak_ptr<BlockingQueueValue> queue)
{
    while (true)
    {
        if (auto ptr = queue.lock())
        {
            auto val = ptr->take(); // WAIT
            if (val.done())
            {
                break;
            }
            std::cout << "bulk: ";
            auto vec = val.vector();
            auto it = vec.begin();
            while (it != vec.end())
            {
                std::cout << *it;
                if (++it != vec.end())
                {
                    std::cout << ", ";
                }
            }
            std::cout << std::endl;
        }
    }
    std::cout << "exit log \n";
};

// вывод в файл
void
file(std::weak_ptr<BlockingQueueValue> queue, std::string prefix)
{
    while (true)
    {
        if (auto ptr = queue.lock())
        {
            auto val = ptr->take(); // WAIT
            if (val.done())
            {
                break;
            }
            auto ts = val.time_stamp();
            std::ofstream file("bulk" + ts.String() + +"." + prefix + ".log");
            if (file.is_open())
            {
                file << "bulk: ";
                auto vec = val.vector();
                auto it = vec.begin();
                while (it != vec.end())
                {
                    file << *it;
                    if (++it != vec.end())
                    {
                        file << ", ";
                    }
                }
                file << std::endl;
            }
        }
        else
        {
            break;
        }
    }
}

class Worker
{
  private:
    struct Done
    {
    };
    error_code m_error{error_ok};
    std::thread m_main_thread;
    std::thread m_log_thread;
    std::thread m_file1_thread;
    std::thread m_file2_thread;
    size_type n{3};
    int m_status_block{no_block_status};
    std::shared_ptr<BlockingQueueValue> log_queue;
    std::shared_ptr<BlockingQueueValue> file_queue;
    std::shared_ptr<PublisherValue> pub;
    std::shared_ptr<Sub> sub;
    std::atomic<bool> m_running{false};
    BlockingQueue<std::variant<Done, std::string>> m_queue;

  public:
    Worker(size_type N)
        : n(N)
    {
        pub = std::make_shared<PublisherValue>();
        log_queue = std::make_shared<BlockingQueueValue>();
        file_queue = std::make_shared<BlockingQueueValue>();
        sub = std::make_shared<Sub>(log_queue, file_queue);
        pub->subscribe(sub);
    }

    ~Worker()
    {
        stop();
    }

    void
    run()
    {
        if (!m_running.exchange(true)) // set true if false
        {
            m_main_thread = std::thread(&Worker::proccess, this);
            m_log_thread = std::thread(&log, log_queue);
            m_file1_thread = std::thread(&file, file_queue, "file1");
            m_file2_thread = std::thread(&file, file_queue, "file2");
        }
    }

    void
    stop()
    {
        if (m_running.exchange(false)) // set false if true
        {
            m_queue.add(Done{});       // EXIT FROM MAIN
            log_queue.get()->add({});  // EXIT FROM LOG
            file_queue.get()->add({}); // EXIT FROM FILE1
            file_queue.get()->add({}); // EXIT FROM FILE2
        }

        if (m_main_thread.joinable())
        {
            m_main_thread.join();
        }

        if (m_log_thread.joinable())
        {
            m_log_thread.join();
        }

        if (m_file1_thread.joinable())
        {
            m_file1_thread.join();
        }

        if (m_file2_thread.joinable())
        {
            m_file2_thread.join();
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
        Status no_block(n, pub);
        auto block_plus = std::make_shared<StatusBlockPlus>(n);
        auto block = std::make_shared<StatusBlock>(n, pub);
        block_plus->subscribe(block);
        while (m_running)
        {
            auto val = m_queue.take(); // block here
            if (!std::holds_alternative<std::string>(val))
            {
                m_error = error_main_proccess_exit;
                break;
            }
            std::string message = std::get<std::string>(val);
            if (message.empty())
            {
                m_error = error_main_proccess_exit;
                break;
            }
            switch (m_status_block)
            {
            default: // no block
                m_status_block = no_block.add(std::move(message));
                break;
            case block_status: // block
                m_status_block = block->add(std::move(message));
                break;
            case block_plus_status: // block ++
                m_status_block = block_plus->add(std::move(message));
                break;
            }
        }
        m_running = false;
        std::cout << "exit main \n";
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
