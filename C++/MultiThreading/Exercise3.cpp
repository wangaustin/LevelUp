// 3. Implement a variant of the program: instead of simply incrementing
//    counter, make the counter wrap every 16 increments (as if 
//    incrementing through indices of an array of length 16).
//    Make two threads each attempt to increment counter (16 * 5000) times.
//    End state should have counter back at index zero.
//    Implement in a buggy naive way that causes counter to often be nonzero,
//    even if atomics are used.

#include <atomic>
#include <iostream>
#include <thread>

std::atomic<int> counter(0);

void threadFunc()
{
    for (int i = 0; i < (16*5000); ++i)
    {
        counter = (counter + 1) % 16;
        //         ^ read   ^modify  ^write
        // BUG!!!
        // Two threads can both read the same value, both compute same result, and write it
        // thereby losing one increment entirely
    }
}

int main()
{
    auto th1 = std::thread(threadFunc);
    auto th2 = std::thread(threadFunc);

    th1.join();
    th2.join();

    std::cout << "counter: " << counter << "\n";
    std::cout << "expected: " << 0 << "\n";
}