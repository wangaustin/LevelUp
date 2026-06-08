#include <array>
#include <atomic>
#include <cstddef>
#include <optional>

// max usable capacity is Capacity - 1 because when next_tail == head, the queue is considered full

template <typename T, size_t Capacity>
class SPSCQueue
{
private:
    // advances circular index
    size_t increment(size_t index) const
    {
        return (index + 1) % Capacity;
    }

    std::array<T, Capacity> buffer_{};

    alignas(64) std::atomic<size_t> head_{0}; // separate head and tail onto different cache lines
    alignas(64) std::atomic<size_t> tail_{0}; // to reduce false sharing; padding the rest to 64 bytes

public:
    
    // pushes one item if queue is not full
    bool push(const T& item)
    {
        const size_t tail = tail_.load(std::memory_order_relaxed); // only atomicity is needed
        const size_t next_tail = increment(tail);

        if (next_tail == head_.load(std::memory_order_acquire)) // nothing after this line can be reordered before it
                                                                // i.e. I'm about to read data, make sure I see everything the writer did
        {
            return false; // queue full
        }

        buffer_[tail] = item;

        tail_.store(next_tail, std::memory_order_release); // nothing before this line can be reordered after it
        // i.e. I'm done writing data, make sure it's all visible before I publish

        return true;
    }

    // pops one item if queue is not empty
    std::optional<T> pop()
    {
        const size_t head = head_.load(std::memory_order_relaxed);

        if (head == tail_.load(std::memory_order_acquire))
        {
            return std::nullopt; // queue empty
        }

        T item = buffer_[head];

        head_.store(increment(head), std::memory_order_release);
        return item;
    }
};

// a release-store synchronizes with a matching acquire-load on the same variable
// - producer pushes to tail, consumer pops the head


// This is a single-producer single-consumer queue, so I do not need compare-exchange. 
// The producer owns tail, the consumer owns head. I still use acquire-release ordering 
// so that when the consumer observes the producer’s updated tail, it also sees the written data. 
// I separate head and tail onto different cache lines to reduce false sharing.

// Why not just use std::queue with a mutex?
// - lock contention, kernel scheduling risk, unpredictable latency spikes, 
//   cache-line bouncing, allocation overhead if container grows

// False sharing
// - False sharing happens when two threads modify different variables that live on the same cache line
// - Even though the variables are different, they may share the same 64-byte cache line
// - If two cores repeatedly modify them, the cache line bounces between cores, causing latency 