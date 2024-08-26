// src/main.cpp
#include "async.h"
#include "blocking_queue.h"
#include "status.h"
#include <fstream>
#include <iostream>
#include <thread>

using BlockingQueueValue = BlockingQueue<Value>;

// получение пачек
struct Sub : public pubsub::Subscriber<Value>
{
    std::weak_ptr<BlockingQueueValue> log;
    std::weak_ptr<BlockingQueueValue> file;
    Sub(std::weak_ptr<BlockingQueueValue> log, std::weak_ptr<BlockingQueueValue> file) : log(log), file(file) {};
    void callback(value_type message) override
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
void log(std::weak_ptr<BlockingQueueValue> queue)
{
    while (true)
    {
        if (auto ptr = queue.lock())
        {
            auto val = ptr->take(); // WAIT
            if (val.done())
            {
                return;
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
};

// вывод в файл
void file(std::weak_ptr<BlockingQueueValue> queue, std::string prefix)
{
    while (true)
    {
        if (auto ptr = queue.lock())
        {
            auto val = ptr->take(); // WAIT
            if (val.done())
            {
                return;
            }
            auto ts = val.time_stamp();
            std::ofstream file("bulk" + ts.String() +
                               +"." + prefix + ".log");
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
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " N" << std::endl;
        return 1;
    }
    std::size_t N = 3;
    try
    {
        N = std::stoi(argv[1]);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Usage: " << argv[0] << " N" << std::endl;
    };
    auto pub = std::make_shared<PublisherValue>();
    auto log_queue = std::make_shared<BlockingQueueValue>();
    auto file_queue = std::make_shared<BlockingQueueValue>();
    auto sub = std::make_shared<Sub>(log_queue, file_queue);
    pub->subscribe(sub);
    std::thread logThread(&log, log_queue);
    std::thread file1Thread(&file, file_queue, "file1");
    std::thread file2Thread(&file, file_queue, "file2");
    try
    {
        Status(N, pub).run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    };
    log_queue.get()->add({}); // EXIT FROM LOG
    file_queue.get()->add({}); // EXIT FROM FILE
    file_queue.get()->add({}); // EXIT FROM FILE
    logThread.join();
    file1Thread.join();
    file2Thread.join();
    return 0;
}
