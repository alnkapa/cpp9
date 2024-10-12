#include "status.h"
#include "async.h"
#include "blocking_queue.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

int
StatusBlockPlus::add(std::string &&command)
{
    if (command == OPEN)
    {
        // auto ptr = std::make_unique<StatusBlockPlus>(N);
        // auto vec = ptr->run();
        // for (auto &&v : vec)
        // {
        //     m_store.emplace_back(v);
        // };
    }
    else if (command == CLOSE)
    {
        // break;
    }
    else
    {
        m_store.emplace_back(std::move(command));
        m_counter++;
        if (m_counter > N)
        {
            m_store.clear();
        };
    }
    return block_plus_status;
}

void
StatusBlock::print()
{
    if (!m_store.empty())
    {
        if (auto ptr = m_pub.lock())
        {
            ptr->notify({m_time_stamp, std::move(m_store)});
        }
    }
}

int
StatusBlock::add(std::string &&command)
{
    if (command == OPEN)
    {
        m_time_stamp.reset();
        return block_plus_status;
        // for (auto &&v : StatusBlockPlus(N, m_ctx).run())
        // {
        //     m_store.emplace_back(v);
        // }
    }
    else if (command == CLOSE)
    {
        print();
        return no_block_status;
    }
    else
    {
        m_time_stamp.update();
        m_store.emplace_back(std::move(command));
        m_counter++;
        if (m_counter > N)
        {
            m_store.clear();
        };
    }
    return block_status;
}

void
Status::print()
{
    if (!m_store.empty())
    {
        if (auto ptr = m_pub.lock())
        {
            ptr->notify({m_time_stamp, std::move(m_store)});
        }
    }
}

int
Status::add(std::string &&command)
{
    if (command == OPEN)
    {
        print();
        m_store.clear();
        m_time_stamp.reset();
        return block_status;
    }
    else if (command != CLOSE)
    {
        m_time_stamp.update();
        m_store.emplace_back(std::move(command));
        if (m_store.size() >= N)
        {
            print();
            m_store.clear();
        }
    }
    return no_block_status;
}
