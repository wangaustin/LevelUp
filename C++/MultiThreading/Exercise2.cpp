// 2. Fix Exercise 1 with atomics

#include <atomic>
#include <iostream>
#include <thread>

std::atomic<int> counter(0);

void threadFunc()
{
    for (int i = 0; i < 5000; ++i)
    {
        counter.fetch_add(1, std::memory_order_relaxed);
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