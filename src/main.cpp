// src/main.cpp
#include "async.h"
#include "blocking_queue.h"
#include "blocking_queue_test.h"
#include <iostream>
#include <thread>

void log()
{
}
void file1()
{
}
void file2()
{
}

int main(int argc, char *argv[])
{
    std::cout << "testAddAndTake:";
    if (testAddAndTake()) {
        std::cout << "TRUE";
    }else{
        std::cout << "FALSE";
    }
    std::cout << "\n";
    std::cout << "testOverflow:";
    if (testOverflow()) {
        std::cout << "TRUE";
    }else{
        std::cout << "FALSE";
    }
    std::cout << "\n";
    // if (argc != 2)
    // {
    //     std::cerr << "Usage: " << argv[0] << " N" << std::endl;
    //     return 1;
    // }
    // int N = std::stoi(argv[1]);
    // Status read(N, Status::BLOCK::OFF);
    
    // std::thread logThread(log);
    // std::thread file1Thread(file1);
    // std::thread file2Thread(file2);
    // logThread.join();
    // file1Thread.join();
    // file2Thread.join();

    // read.reader();
    return 0;
}
