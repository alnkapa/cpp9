#pragma once
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../global/hash.h"

namespace arg {
    struct Argc {
        std::vector<std::string> directory;  // директории для сканирования
        std::vector<std::string> directory_exception;  // директории для исключения из сканирования
        int deep_level;                                // уровень сканирования
        std::size_t file_size;                         // минимальный размер файла
        std::string wildcards;                         // маски имен файлов
        std::size_t block_size;                        // размер блока
        ::hash::Algorithm hashes;                      // алгоритмов хэширования
        int parse(int argc, char *argv[]);
    };
};  // namespace arg

namespace boost {
    template <>
    inline ::hash::Algorithm lexical_cast<::hash::Algorithm>(const std::string &arg) {
        if (arg == "crc32") {
            return ::hash::Algorithm::CRC32;
        } else if (arg == "md5") {
            return ::hash::Algorithm::MD5;
        } else if (arg == "sha1") {
            return ::hash::Algorithm::SHA1;
        } else if (arg == "sha256") {
            return ::hash::Algorithm::SHA256;
        } else {
            throw std::invalid_argument("Invalid hash algorithm. Allowed values are: crc32 or md5 or sha1 or sha256");
        }
    };
    template <>
    inline std::string lexical_cast<std::string>(const ::hash::Algorithm &arg) {
        switch (arg) {
            case ::hash::Algorithm::CRC32:
                return "crc32";
            case ::hash::Algorithm::MD5:
                return "md5";
            case ::hash::Algorithm::SHA1:
                return "sha1";
            case ::hash::Algorithm::SHA256:
                return "sha256";
            default:
                throw std::invalid_argument("Invalid hash algorithm");
        };
    };
}  // namespace boost