#ifndef STATUS_H
#define STATUS_H
#include <iostream>
#include <chrono>
#include <vector>
#include <tuple>
#include "pub_sub.h"

using unix_time_stamp_t = long long;
using print_t = std::pair<unix_time_stamp_t, std::vector<std::string>>;

class Status : private Publisher<print_t>
{
public:
    enum class BLOCK : int
    {
        OFF = 0,
        ON = 1,
        INNER = 2
    };
    using Publisher<print_t>::subscribe;
    using Publisher<print_t>::unsubscribe;
    
private:
    std::vector<std::string> m_stack;
    long long m_time_stamp{0};
    int m_n{3};
    int m_counter{0};
    BLOCK m_block_hierarchy{BLOCK::OFF};
    bool m_block_end{false};
    void print()
    {
        if (m_stack.size() > 0)
        {
            notify({m_time_stamp, m_stack});
        }
    };

    void clear()
    {
        m_stack.clear();
        m_counter = 0;
        m_time_stamp = 0;
    };

public:
    Status(int N, BLOCK block_status) : m_n(N), m_block_hierarchy(block_status) {};
    ~Status()
    {
        if (m_block_hierarchy == BLOCK::OFF)
        {
            print();
        }
        clear();
    };

    void reader()
    {
        std::string command;
        while (std::getline(std::cin, command))
        {
            if (command.empty())
            {
                break;
            }
            if (command == "{")
            {
                if (m_block_hierarchy == BLOCK::OFF)
                {
                    print();
                    clear();
                }
                // new block
                auto block_hierarchy = m_block_hierarchy;
                if (m_block_hierarchy < BLOCK::INNER)
                {
                    block_hierarchy = static_cast<BLOCK>(static_cast<int>(m_block_hierarchy) + 1);
                }
                Status inner_reader(m_n, block_hierarchy);
                inner_reader.reader();
                if (m_block_hierarchy > BLOCK::OFF)
                {
                    for (auto &&v : inner_reader.m_stack)
                    {
                        m_stack.emplace_back(std::move(v));
                    }
                }
                else if (inner_reader.m_block_end)
                {
                    inner_reader.print();
                }
                // call dtor
            }
            else if (command == "}")
            {
                m_block_end = true;
                // close block
                if (m_block_hierarchy != BLOCK::OFF)
                {
                    return;
                }
            }
            else
            {
                if (m_time_stamp == 0)
                {
                    m_time_stamp = std::chrono::duration_cast<std::chrono::seconds>(
                                       std::chrono::system_clock::now().time_since_epoch())
                                       .count();
                }
                // in block
                m_stack.emplace_back(std::move(command));
                m_counter++;
                if (m_block_hierarchy == BLOCK::OFF && m_counter == m_n)
                {
                    print();
                    clear();
                }
                else if (m_block_hierarchy > BLOCK::OFF && m_counter > m_n)
                {
                    clear();
                }
            }
        }
        return;
    };
};

#endif