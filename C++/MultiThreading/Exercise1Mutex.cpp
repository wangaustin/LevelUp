#include <thread>
#include <iostream>
#include <mutex>

int counter = 0;
std::mutex counterMutex;

void increment(int n) {
    for (int i = 0; i < n; ++i)
        std::lock_guard<std::mutex> lock(counterMutex);
        ++counter;   // UB: unsynchronized write
}

int main() {
    std::thread t1(increment, 100000);
    std::thread t2(increment, 100000);
    t1.join(); t2.join();
    std::cout << counter;
}