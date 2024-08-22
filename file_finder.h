#ifndef FILE_FINDER_H
#define FILE_FINDER_H

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/key.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/key.hpp>
#include "global/hash.h"
#include "hashing/hash.h"
#include "argc/argc.h"
#include "files/files.h"

using namespace boost::multi_index;
class proccess
{
  private:
    // clang-format off
    typedef multi_index_container<
        files::File,
        indexed_by<hashed_unique<key<&files::File::path>>, // поиск файла
        ordered_non_unique<key<&files::File::size>>   // поиск размера
    >> container_t;
    // clang-format on
    container_t container;
    // хеш функция
    std::unique_ptr<hash::HasherBase<hash::HashTypeImpl>> h_fu;
    // параметры запуска
    std::size_t m_block_size; // размер блока
    // временное хранилище для блоков сравниваемого файла
    std::vector<hash::HashTypeImpl> m_blocks_path1{};
    // сравнение файлов по блочно
    // 
    // path1 новый файл
    // path2 файл из индекса
    bool block_cmp(const std::string &path1, const std::string &path2, std::uintmax_t file_size);
    //  получить хеш блока
    hash::HashTypeImpl block_hash(const std::string &path, const size_t b_num);


  public:
    proccess(std::size_t block_size, hash::Algorithm hashes) : m_block_size(block_size)
    {
        h_fu = hash::CreateHasher<hash::HashTypeImpl>(hashes);
    };
    // выбрать из индекса файлы одинаковые по размеру
    void file_cmp(const std::string &path, std::uintmax_t file_size);
    // результат
    void print_result();
};

#endif