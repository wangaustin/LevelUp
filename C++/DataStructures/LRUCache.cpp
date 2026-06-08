#include <list>
#include <unordered_map>

class LRUCache
{
private:
    using Node = std::pair<int, int>; // {key, value}

    int capacity_;

    // front = most recently used
    // back = least recently used
    std::list<Node> items_;

    // key -> iterator pointing to the list
    std::unordered_map<int, std::list<Node>::iterator> map_;


public:
    LRUCache(int capacity) : capacity_(capacity) {}

    int get(int key)
    {
        auto it = map_.find(key);

        if (it == map_.end()) return -1;

        // key was accessed, so it becomes the most recently used
        // splice moves existing list node to the front in O(1)
        items_.splice(items_.begin(), items_, it->second);

        // it->second still points to the same node, though it's now at the front
        return it->second->second;
    }

    void put(int key, int value)
    {
        auto it = map_.find(key);

        if (it != map_.end())
        {
            // key already exists, update value in the list node
            it->second->second = value;

            // since we touched this key, it's now most recently used
            items_.splice(items_.begin(), items_, it->second);
            return;
        }

        // key does not exist
        // insert at front because new entries are most recently used
        items_.emplace_front(key, value);

        // store iterator to new front node
        map_[key] = items_.begin();

        if (items_.size() > capacity_)
        {
            // back of list is least recently used item
            auto lru = items_.back();

            // remove back of list from hash map first
            map_.erase(lru.first);

            // remove back of list from list
            items_.pop_back();
        }
    }
};