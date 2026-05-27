// 1. Implement a program that uses two threads to increment a global
//    counter to 10,000 with each thread incrementing 5,000 
//    BUT make it buggy so that there are interleaving problems
//    and the end result is <10,000

#include <iostream>
#include <thread>

int counter = 0;

void threadFunc()
{
    for (int i = 0; i < 5000; ++i)
    {
        ++counter;
    }
}

int main()
{
    auto th1 = std::thread(threadFunc);
    auto th2 = std::thread(threadFunc);

    th1.join();
    th2.join();

    std::cout << "counter: " << counter << "\n";
    std::cout << "expected: " << "10,000" << "\n";
}