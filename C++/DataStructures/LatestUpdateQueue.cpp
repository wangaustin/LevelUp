#include <unordered_map>
#include <optional>
#include <stdexcept>
#include <utility>
#include <list>

#include <iostream>
#include <string>


template <typename T, typename Key, typename KeyFn>
class LatestUpdateQueue
{
private:
    KeyFn key_fn_; // extracts coalescing key from item; e.g. for Instrument, this could return its symbol
    std::list<Key> order_; // stores keys in order they first became pending, a key appears here at most once
    std::unordered_map<Key, T> latest_; // maps each pending key to its latest item, newer pushes for same key overwrites old

public:
    explicit LatestUpdateQueue(KeyFn key_fn_)
        : key_fn_(std::move(key_fn_)) {}

    // adds an update; if this key is already pending, only keep latest value
    void push(T item)
    {
        Key key = key_fn_(item);

        // first time this key is pending, remember its FIFO position
        if (latest_.find(key) == latest_.end())
        {
            order_.push_back(key);
        }

        // store newest update for this key
        latest_[key] = std::move(item);
    }

    // pop oldest pending key, but returns latest update
    T pop()
    {
        if (empty())
        {
            throw std::runtime_error("pop from empty LatestUpdateQueue");
        }

        Key key = std::move(order_.front());
        order_.pop_front();

        T result = std::move(latest_.at(key));
        latest_.erase(key);

        return result;
    }

    // try to pop without throwing
    std::optional<T> try_pop()
    {
        if (empty()) return std::nullopt;

        return pop();
    }

    // try when there's no pending unique keys
    bool empty() const
    {
        return order_.empty();
    }

    // number of unique pending keys, not total pushes
    size_t size() const
    {
        return order_.size();
    }
};

struct Instrument
{
    std::string symbol;
    double price;
};

int main()
{
    auto get_symbol = [](const Instrument& i)
    {
        return i.symbol;
    };

    LatestUpdateQueue<Instrument, std::string, decltype(get_symbol)> q(get_symbol);

    q.push({"AAPL", 100.0});
    q.push({"MSFT", 200.0});
    q.push({"AAPL", 101.5});
    q.push({"NVDA", 900.0});
    q.push({"MSFT", 202.0});

    auto first = q.pop();
    auto second = q.pop();
    auto third = q.pop();

    std::cout << first.symbol << " " << first.price << "\n"; // AAPL 101.5
    std::cout << second.symbol << " " << second.price << "\n"; // MSFT 202.0
    std::cout << third.symbol << " " << third.price << "\n"; // NVDA 900.0
}