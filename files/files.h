#ifndef BLOCK_H
#define BLOCK_H
#include <vector>

#include "../hashing/hash_type.h"

namespace files
{

class File
{
  public:
    std::uintmax_t size{0}; // размер файла
    std::string path{};     // путь до файла
  private:
    std::vector<hash::HashTypeImpl> m_blocks{}; // массив блоков в файле
    std::vector<std::string> m_paths{};         // массив идентичных файлов
  public:
    explicit File(const std::string &path, const std::uintmax_t &size, std::vector<hash::HashTypeImpl> &&blocks)
        : path(path), size(size), m_blocks(std::move(blocks)) {};

    // получить хеш
    const hash::HashTypeImpl &at(std::size_t num) const
    {
        return m_blocks.at(num);
    };
    // получить хеш
    hash::HashTypeImpl &at(std::size_t num)
    {
        // TODO : at double call
        try
        {
            m_blocks.at(num);
        }
        catch (const std::out_of_range &ex)
        {
            m_blocks.resize(num + 1);
        }
        return m_blocks.at(num);
    };
    const std::vector<std::string> &paths() const
    {
        return m_paths;
    };
    void push_paths(const std::string &in)
    {
        m_paths.push_back(in);
    };
};

} // namespace files
#endif // BLOCK_H