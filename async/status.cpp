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
StatusBlockPlus::add(std::string &&line)
{
    if (line == OPEN)
    {
        ++m_level;
        if (!m_stop)
        {
            m_stack.emplace_back(std::move(m_store), m_counter);
            m_counter = 0;
        }
    }
    else if (line == CLOSE)
    {
        if (m_stop && m_stop_level == m_level)
        {
            m_stop = false;
            m_stop_level = 0;
        }

        if (m_level > 0)
        {
            --m_level;

            if (!m_stop)
            {
                if (!m_stack.empty())
                {
                    auto pred = std::move(m_stack.back());
                    m_stack.pop_back();
                    // m_store = pred.first + m_store;
                    std::vector<std::string> n_store(m_store.size() +
                                                     pred.first.size());
                    std::copy(pred.first.begin(), pred.first.end(), n_store.begin());
                    std::copy(m_store.begin(), m_store.end(),
                              n_store.begin() + pred.first.size());
                    m_store = std::move(n_store);
                    m_counter = pred.second;
                }
            }
        }
        else
        {
            if (!m_stop)
            {
                if (!m_store.empty())
                {
                    if (auto ptr = m_pub.lock())
                    {
                        ptr->notify(std::move(m_store));
                    }
                }
                m_store.clear();
                m_counter = 0;
            }
            return block_status;
        }
    }
    else if (!m_stop)
    {
        m_store.emplace_back(std::move(line));
        ++m_counter;
        if ((m_stop = m_counter > N))
        {
            m_stop_level = m_level;
            m_store.clear();
            m_counter = 0;
        }
    }
    return block_plus_status;
}

void
StatusBlock::clear()
{
    m_store.clear();
    m_time_stamp.reset();
    m_counter = 0;
}

void
StatusBlock::callback(std::vector<std::string> in)
{
    // m_store =  m_store + in;
    std::vector<std::string> n_store(m_store.size() + in.size());
    // TODO: !!! use constructor with pos !!!
    // n_store{pos=m_store.size(), std::move(in)}
    std::copy(m_store.begin(), m_store.end(), n_store.begin());
    std::copy(in.begin(), in.end(), n_store.begin() + m_store.size());
    m_store = std::move(n_store);
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
StatusBlock::add(std::string &&line)
{
    if (line == OPEN)
    {
        return block_plus_status;
    }
    else if (line == CLOSE)
    {
        if (m_counter <= N)
        {
            print();
        }
        clear();
        return no_block_status;
    }
    else
    {
        m_time_stamp.update();
        m_store.emplace_back(std::move(line));
        m_counter++;
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
Status::add(std::string &&line)
{
    if (line == OPEN)
    {
        print();
        m_store.clear();
        m_time_stamp.reset();
        return block_status;
    }
    else if (line != CLOSE)
    {
        m_time_stamp.update();
        m_store.emplace_back(std::move(line));
        if (m_store.size() >= N)
        {
            print();
            m_store.clear();
            m_time_stamp.reset();
        }
    }
    return no_block_status;
}
