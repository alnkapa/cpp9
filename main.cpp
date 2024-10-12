// src/main.cpp
#include "async.h"
#include <exception>
#include <iostream>
#include <memory>
#include <string>

int
main(int argc, char *argv[])
{
    // if (argc != 2) {
    //   std::cerr << "Usage: " << argv[0] << " N" << std::endl;
    //   return 1;
    // }
    std::size_t N = 3;
    try
    {
        auto ptr =
            std::unique_ptr<async::context_type, void (*)(async::context_type *)>(
                new async::context_type(async::connect(N)),
                [](async::context_type *p)
                {
                    async::disconnect(*p);
                    delete p;
                });

        std::string command{};
        while (std::getline(std::cin, command))
        {
            auto err = async::receive(*ptr, command.data(), command.size());
            if (err != async::error_ok)
            {
                std::cout << "async error : " << err << "\n";
                break;
            }
        }
    }
    catch (const std::exception &err)
    {
        std::cout << "error : " << err.what() << "\n";
    }
    return 0;
}
