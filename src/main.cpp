// src/main.cpp
#include "async.h"
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

int main()
{

    std::thread logThread(log);
    std::thread file1Thread(file1);
    std::thread file2Thread(file2);
    logThread.join();
    file1Thread.join();
    file2Thread.join();
    return 0;
}
