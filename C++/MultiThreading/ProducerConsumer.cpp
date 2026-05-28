#include <queue>
#include <mutex>
#include <condition_variable>
#include <utility>

template<typename T, size_t N>
class BoundedQueue
{
private:
    std::queue<T> q;
    std::mutex mtx;
    std::condition_variable not_full; // producer
    std::condition_variable not_empty; // consumer

public:
    void push(T item)
    {
        std::unique_lock<std::mutex> lock(mtx);

        // wait until there is space
        // predicate protects against spurious wakeups
        not_full.wait(lock, [this]
        {
            return q.size() < N;
        });

        q.push(std::move(item));

        // wait one waiting consumer
        not_empty.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(mtx);

        not_empty.wait(lock, [this]
        {
            return !q.empty();
        });

        T item = std::move(q.front());

        q.pop();

        // wait one waiting producer
        not_full.notify_one();

        return item;
    }
};