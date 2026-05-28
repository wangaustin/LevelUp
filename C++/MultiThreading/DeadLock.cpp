// Two threads acquire two locks in opposite order. Explain why this deadlocks and show two ways to fix it.

#include <mutex>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;


// PROBLEM: circular wait

std::mutex m1, m2;

void thread_a() {
    std::lock_guard l1(m1);
    std::this_thread::sleep_for(1ms);
    std::lock_guard l2(m2); // waits for m2
}
void thread_b() {
    std::lock_guard l1(m2);
    std::this_thread::sleep_for(1ms);
    std::lock_guard l2(m1); // waits for m1 -> deadlock
}


// FIX #1: always lock mutexes in the same order

#include <mutex>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

void thread_a_1()
{
    std::lock_guard l1(m1);
    std::this_thread::sleep_for(1ms);
    std::lock_guard l2(m2);
}

void thread_b_1()
{
    std::lock_guard l1(m1);
    std::this_thread::sleep_for(1ms);
    std::lock_guard l2(m2);
}


// FIX #2: use scoped_lock()

#include <mutex>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

void thread_a_2()
{
    std::scoped_lock l(m1, m2);
    // critical section using both resources
}

void thread_b_2()
{
    std::scoped_lock l(m1, m2);
}