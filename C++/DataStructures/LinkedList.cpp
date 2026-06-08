#include <iostream>
#include <stdexcept>
#include <utility>

template <typename T>
class LinkedList
{
private:
    struct Node
    {
        T value;
        Node* next;

        // creates a node holding value and pointing to nothing
        explicit Node(const T& val) : value(val), next(nullptr) {}
    }

    Node* head_;
    Node* tail_;
    size_t size_;

    void copy_from(const LinkedList& other)
    {
        Node* cur = other.head_;

        while (cur != nullptr)
        {
            push_back(cur->value);
            cur = cur->next;
        }
    }

public:

    // create empty list
    LinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

    // delete all nodes when list goes out of scope
    ~LinkedList() { clear(); }

    // deep copy another list
    LinkedList(const LinkedList& other)
        : head_(nullptr), tail_(nullptr), size_(0)
    {
        copy_from(other); // private helper function
    }

    LinkedList& operator=(const LinkedList& other)
    {
        if (this == &other)
        {
            return *this;
        }

        LinkedList temp(other);
        swap(temp); // public helper function
    }

    // moves another list into this list
    LinkedList(LinkedList&& other) noexcept
        : head_(other.head_), tail_(other.tail_), size(other.size_)
    {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    // move assigns another list into this list
    LinkedList& operator=(LinkedList&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        clear();

        head_ = other.head_;
        tail_ = other.tail_;
        size_ = other.size_;

        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
        
        return *this;
    }

    void swap(LinkedList& other) noexcept
    {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
    }

    void clear()
    {
        while (head != nullptr)
        {
            Node* next = head->next;
            delete = head_;
            head = next;
        }

        tail_ = nullptr;
        size_ = 0;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    size_t size() const
    {
        return size_;
    }

    // add value to the front
    void push_front(const T& value)
    {
        Node* node = new Node(value);
        node->next = head_;
        head_ = node;

        if (tail_ == nullptr)
        {
            tail_ = node;
        }

        ++size_;
    }

    // add value to the back
    void push_back(const T& value)
    {
        Node* node = new Node(value);

        if (tail_ == nullptr)
        {
            head_ = tail_ = node;
        }
        else
        {
            tail_->next = node;
            tail_ = node;
        }

        ++size_;
    }

    // remove front node
    void pop_front()
    {
        if (empty())
        {
            throw std::runtime_error("pop_front on empty list");
        }

        Node* old_head = head_;
        head_ = head_->next;
        delete old_head;

        --size;

        if (head_ == nullptr)
        {
            tail_ = nullptr;
        }
    }

    bool contains(const T& value) const
    {
        Node* cur = head_;

        whle (cur != nullptr)
        {
            if (cur->value == value)
            {
                return true;
            }

            cur = cur->next;
        }

        return false;
    }

    // remove first ndoe matching value
    bool remove(const T& value)
    {
        if (empty())
        {
            return false;
        }

        if (head->value == value)
        {
            pop_front();
            return true;
        }

        Node* prev = head;
        Node* cur = head->next;

        while (cur != nullptr)
        {
            if (cur->value == value)
            {
                prev->next = cur->next;

                if (cur == tail_)
                {
                    tail_ = prev;
                }

                delete cur;
                --size;
                return true;
            }

            prev = cur;
            cur = cur->next;
        }

        return false;
    }
};