#include "file_finder.h"
#include <iostream>
#include <fstream>

inline std::size_t get_block_number(std::uintmax_t file_size, std::size_t block_size) noexcept
{
    auto block_number = file_size / block_size;
    if (file_size % block_size != 0)
    {
        block_number++;
    }
    return block_number;
}

struct BlockSave
{
    BlockSave(const size_t index, const hash::HashTypeImpl &value) : m_index(index), m_value(value)
    {
    }

    void operator()(files::File &e)
    {
        e.at(m_index) = m_value;
    }

  private:
    size_t m_index;
    hash::HashTypeImpl m_value;
};

struct FileSave
{
    FileSave(const std::string &value) : m_value(value)
    {
    }

    void operator()(files::File &e)
    {
        e.push_paths(m_value);
    }

  private:
    std::string m_value;
};

hash::HashTypeImpl proccess::block_hash(const std::string &path, const size_t b_num)
{
    auto file = std::fstream(path, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + path);
    }
    char buf[m_block_size];
    file.seekg(b_num * m_block_size);
    auto size = file.read(reinterpret_cast<char *>(&buf), m_block_size).gcount();
    if (size == 0 || size > m_block_size)
    {
        throw std::length_error("block size mismatch: " + std::to_string(size));
    }
    return h_fu->hash({reinterpret_cast<hash::value_type *>(&buf), static_cast<hash::size_type>(size)});
}

bool proccess::block_cmp(const std::string &path1, const std::string &path2, std::uintmax_t file_size)
{
    // по блочное сравнение
    auto block_number = get_block_number(file_size, m_block_size);
    auto &file_index = container.get<0>();
    auto file_name_it = file_index.find(path2);
    if (file_name_it == file_index.end())
    {
        throw std::invalid_argument("file not fount: " + path2);
    }
    for (std::size_t b_num = 0; b_num < block_number; ++b_num)
    {
        try
        {
            m_blocks_path1.at(b_num);
        }
        catch (const std::out_of_range &ex)
        {
            m_blocks_path1.resize(b_num + 1);
            m_blocks_path1.at(b_num) = block_hash(path1, b_num);
        }

        try
        {
            file_name_it->at(b_num);
        }
        catch (const std::out_of_range &ex)
        {
            file_index.modify(file_name_it, BlockSave(b_num, block_hash(path1, b_num)));
        }

        // TODO : at double call
        if (m_blocks_path1.at(b_num) != file_name_it->at(b_num))
        {
            // блоки не совпали
            return false;
        }
    }
    return true;
}

void proccess::file_cmp(const std::string &path, std::uintmax_t file_size)
{
    m_blocks_path1.clear();
    // найти похожие файлы по размеру
    auto &file_size_index = container.get<1>();
    auto file_size_range_it = file_size_index.equal_range(file_size);
    if (file_size_range_it.first == file_size_range_it.second)
    {
        // совпадений по размеру не найдено
        container.emplace(path, file_size, std::move(m_blocks_path1));
        return;
    }
    bool is_find = false;
    for (auto &file_size_it = file_size_range_it.first; file_size_it != file_size_range_it.second; ++file_size_it)
    {
        // TODO: parallel ?
        if (block_cmp(path, file_size_it->path, file_size))
        {
            // найдено совпадение
            is_find = true;
            file_size_index.modify(file_size_it, FileSave(path));
            break;
        }
    }
    if (!is_find)
    {
        // совпадений по блокам не найдено
        container.emplace(path, file_size, std::move(m_blocks_path1));
    }
}

void proccess::print_result()
{
    auto &file_index = container.get<0>();
    for (auto file_it = file_index.begin(); file_it != file_index.end(); ++file_it)
    {        
        auto same = file_it->paths();
        if (!same.empty())
        {
            std::cout << file_it->path << "\n";
            for (const auto &v : same)
            {
                std::cout << v << "\n";
            }
            std::cout << "\n";
        }
    }
}
