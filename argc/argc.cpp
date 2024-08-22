#include "argc.h"

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <regex>

namespace arg
{
int Argc::parse(int argc, char *argv[])
{
    namespace po = boost::program_options;
    // clang-format off
        // Определяем параметры командной строки
        po::options_description desc("Allowed options");
        desc.add_options()
        (
            "help,h", 
            "produce help message"
        )
        (
            "directory,d", 
            po::value<std::vector<std::string>>(&directory)->required(),
            "input directory (for multiples input use -d dir1 -d dir2 -d dir3 ...)"
        )
        (
            "exception,e", 
            po::value<std::vector<std::string>>(&directory_exception),
            "exception directory (for multiples input use -e dir1 -e dir2 -e dir3 ...)"
        )
        (
            "level,l", 
            po::value<int>(&deep_level)->default_value(0),
            "deep level"
        )
        (
            "size,s", 
            po::value<std::size_t>(&file_size)->default_value(1),
            "min file size in byte"
        )
        (
            "wildcards,w", 
            po::value<std::string>(&wildcards),
            "file name wildcards variants *.cpp, file?.txt \n"
            "An asterisk (*) - matches one or more occurrences of any character, including no character \n"
            "Question mark (?) - represents or matches a single occurrence of any character. \n"
        )
        (
            "bsize,S", 
            po::value<std::size_t>(&block_size)->default_value(80), 
            "block size in byte"
        )
        (
            "hashes,H", 
            po::value<::hash::Algorithm>(&hashes)->default_value(::hash::Algorithm::CRC32), 
            "hash algorithm (crc32 or md5 or sha1 or sha256)"
        );
    // clang-format on
    po::variables_map vm;
    std::string out{};
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return -2;
        }

        if (!wildcards.empty())
        {
            out.reserve(wildcards.size() * 2);
            std::ranges::for_each(wildcards,
                                  [&out](const char &c)
                                  {
                                      char escape{'\\'};
                                      switch (c)
                                      {
                                      case '.':
                                          [[fallthrough]];
                                      case '^':
                                          [[fallthrough]];
                                      case '$':
                                          [[fallthrough]];
                                      case '+':
                                          [[fallthrough]];
                                      case '{':
                                          [[fallthrough]];
                                      case '}':
                                          [[fallthrough]];
                                      case '(':
                                          [[fallthrough]];
                                      case ')':
                                          [[fallthrough]];
                                      case '|':
                                          [[fallthrough]];
                                      case '\\':
                                          [[fallthrough]];
                                      case '[':
                                          [[fallthrough]];
                                      case ']':
                                          out.push_back(escape);
                                          out.push_back(c);
                                          break;
                                      case '*':
                                          out.push_back('.');
                                          out.push_back('*');
                                          break;
                                      case '?':
                                          out.push_back('.');
                                          break;
                                      default:
                                          out.push_back(c);
                                          break;
                                      }
                                  });

            std::regex re(out);
            wildcards = out;
        }
    }
    catch (const po::required_option &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << desc << "\n";
        return -1;
    }
    catch (const po::error &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << desc << "\n";
        return -1;
    }
    catch (const std::regex_error &e)
    {
        std::cout << out << " regex_error caught: " << e.what() << '\n';
    }
    // Выводим значения аргументов
    // std::cout << "Input directories: ";
    // std::copy(directory.begin(), directory.end(), std::ostream_iterator<std::string>(std::cout, " "));
    // std::cout << "\n";
    // if (!directory_exception.empty()) {
    //     std::cout << "Exception directories: ";
    //     std::copy(directory_exception.begin(), directory_exception.end(),
    //               std::ostream_iterator<std::string>(std::cout, " "));
    // }
    // std::cout << "\n";
    // std::cout << "Level: " << deep_level << "\n";
    // std::cout << "Size: " << file_size << "\n";
    // std::cout << "Block size: " << block_size << "\n";
    // std::cout << "Hash algorithm: " << boost::lexical_cast<std::string>(hashes) << "\n";
    return 0;
};
}; // namespace arg
