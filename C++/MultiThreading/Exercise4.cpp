// 4. Fix Exercise 3 using a CAS loop

#include <atomic>
#include <thread>
#include <iostream>

std::atomic<int> counter(0);

void threadFunc()
{
    for (int i = 0; i < (16*50000); ++i)
    {
        bool success = false;

        do
        {
            int counter_old = counter.load(); // snapshot current value
            int counter_new = (counter_old + 1) % 16; // compute desired new value
            success = counter.compare_exchange_weak(counter_old, counter_new);
        } while (!success);
    }
}

// why not std::compare_exchange_strong?
// because we need CAS loop anyway to retry in case of contention, so might as well
// use compare_exchange_weak for additional performance

int main()
{
    auto th1 = std::thread(threadFunc);
    auto th2 = std::thread(threadFunc);

    th1.join();
    th2.join();

    std::cout << "counter: " << counter << "\n";
    std::cout << "expected: " << 0 << "\n";
}