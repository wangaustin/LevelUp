#include <cstdint>
#include <map>
#include <unordered_map>

enum class Type
{
    Add,
    Delete
};

struct Message
{
    uint64_t seq;
    Type type;
    uint64_t order_id;
    uint32_t price;
    uint32_t qty;
    bool is_buy;
};

struct Order
{
    uint32_t price;
    uint32_t qty;
    bool is_buy;
};

class MarketDataProcessor
{
private:
    uint64_t expected_seq_;
    bool stale_ = false;

    std::unordered_map<uint64_t, Order> orders_;
    std::map<uint32_t, uint32_t> bids_;
    std::map<uint32_t, uint32_t> asks_;

    // applies a validated in-sequence message to the book
    void apply(const Message& msg)
    {
        if (msg.type == Type::Add)
        {
            add_order(msg);
        }
        else if (msg.type == Type::Delete)
        {
            delete_order(msg.order_id);
        }
    }

    // add an order and updates aggregate price level
    void add_order(const Message& msg)
    {
        orders_[msg.order_id] = Order{
            msg.price,
            msg.qty,
            msg.is_buy
        };

        auto& side = msg.is_buy ? bids_ : asks_;

        side[msg.price] += msg.qty;
    }

    void delete_order(uint64_t order_id)
    {
        auto it = orders_.find(order_id);
        if (it == orders_.end()) return; // order not found

        const Order& order = it->second;
        auto& side = order.is_buy ? bids_ : asks_;

        auto level_it = side.find(order.price);
        if (level_it != side.end())
        {
            level_it->second -= order.qty;
            if (level_it->second == 0)
            {
                side.erase(level_it);
            }
        }

        orders_.erase(it);
    }

public:
    explicit MarketDataProcessor(uint64_t first_seq) : expected_seq_(first_seq) {}

    // applies one message if it's the next expected sequence
    void on_message(const Message& msg)
    {
        if (msg.seq < expected_seq_)
        {
            return; // duplicate or stale message
        }

        if (msg.seq > expected_seq_)
        {
            stale_ = true;
            return;
        }

        apply(msg);
        ++expected_seq_;
    }

    // returns highest bid price, or 0 if none exists
    uint32_t best_bid() const
    {
        if (bids_.empty())
        {
            return 0;
        }

        return bids_.rbegin()->first;
    }

    // returns lowest ask price, or 0 if none exists
    uint32_t best_ask() const
    {
        if (asks_.empty())
        {
            return 0;
        }

        return asks_.begin()->first;
    }

    // returns whether the book can no longer be trusted
    bool is_stale() const
    {
        return stale_;
    }
};