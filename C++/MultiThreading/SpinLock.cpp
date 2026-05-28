#include <atomic>
#include <thread>

class SpinLock
{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    void lock()
    {
        // test_and_set() returns old value
        // - if FALSE, set it to true and acquire the lock
        // - if TRUE, someone else owns the lock, keep spinning
        while (flag.test_and_set(std::memory_order_acquire))
        {
            // reduce CPU pressure a little bit while spinning
            // useful under contention but not a substitute for a mutex
            std::this_thread::yield();
        }
    }

    // why memory_order_acquire?
    // after this thread acquires the lock, it must see all memory writes 
    // made by previous lock owner before that owner called unlock()
    // prevents reads/writes inside critical section from being reordered
    // before lock acquisition

    void unlock()
    {
        // release lock
        // memory_order_release ensures writes inside critical section 
        // become visible before another thread acquires the lock
        flag.clear(std::memory_order_release);
    }

    // why memory_order_release?
    // all writes done inside critical section must become visible before 
    // lock is marked as available
};

// when is spinlock better than std::mutex?
// 1. critical section is short
// 2. contention is expected to be very low
// 3. blocking the thread would cost more than briefly spinning
// 4. low-level system code where sleeping is undesirable or impossible