#include <vector>
#include <optional>
#include <cstddef>

template <typename T>
class RingBuffer
{
private:
    std::vector<T> data_;
    size_t capacity_ = 0; // full means size_ == capacity_
    size_t head_ = 0; // points to oldest readable element
    size_t tail_ = 0; // points to next write position
    size_t size_ = 0; // empty means size_ == 0

    // wraps an index around the fixed-sized array
    size_t next(size_t index) const
    {
        return (index + 1) % capacity_;
    }

public:
    explicit RingBuffer(size_t capacity)
        : data_(capacity), capacity_(capacity) {}

    // place value at tail_ and advance tail_ modulo capacity
    void write(const T& value)
    {
        if (capacity_ == 0) return;

        data_[tail_] = value;
        tail_ = next(tail_);

        // buffer full
        if(size_ == capacity_)
        {
            // we overwrote the oldest element, so move head forward
            head_ = next(head_);
        }
        else
        {
            ++size_;
        }
    }

    // move-aware write
    void write(T&& value)
    {
        if (capacity_ == 0) return;

        data_[tail_] = std::move(value);
        tail_ = next(tail_);

        if (size_ == capacity_)
        {
            head_ = next(head_);
        }
        else
        {
            ++size_;
        }
    }

    // returns oldest without removing, empty returns nullptr
    const T* read() const
    {
        if (empty()) return nullptr;

        return &data_[head_];
    }

    // copies oldest item into out without removing it
    bool try_read(T& out) const
    {
        if (empty()) return false;

        out = data_[head_];
        return true;
    }

    // removes oldest item
    bool pop()
    {
        if (empty()) return false;

        head_ = next(head_);
        --size_;
        return true;
    }

    // reads and removes oldest item
    bool try_pop(T& out)
    {
        if (empty()) return false;

        out = std::move(data_[head_]);
        head_ = next(head_);
        --size_;

        return true;
    }

    // removes all elements from buffer
    void reset()
    {
        head_ = 0;
        tail_ = 0;
        size_ = 0;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    bool full() const
    {
        return size_ == capacity_;
    }

    size_t size() const
    {
        return size_;
    }

    size_t capacity() const
    {
        return capacity_;
    }
};