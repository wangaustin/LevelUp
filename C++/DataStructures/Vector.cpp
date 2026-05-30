#include <cstddef> // size_t
#include <stdexcept> // std::out_of_range
#include <utility> // std::move, std::swap, std::forward

template<typename T>
class Vector
{
private:
    T* data_;           // pointer to raw storage
    size_t size_;       // number of current elements
    size_t capacity_;   // number of allocated slots

    // allocate new memory and copy existing element
    void reallocate(size_t new_capacity)
    {
        T* new_data = new T[new_capacity];

        for (size_t i = 0; i < size_; ++i)
        {
            new_data[i] = std::move(data_[i]);
            // copying is expensive, and for std::unique_ptr, it's not even allowed
            // also, move is faster for std::string, std::vector<int>, std::unique_ptr<T>, or large custom classes
            
            // why not use std::memcpy?
            // because it only copies raw bytes and is only safe for primitives like int, double, or simple structs
            // it bypasses constructors, copy constructors, move constructors, and ownership logic in general
            // for example:
            // std::string a = "hi";
            // std::string b;
            // std::memcpy(&b, &a, sizeof(std::string)); // WRONG!! only copies internal pointer bytes, not real string copy
            //                                           // both strings may point to same internal bugger, may cause
            //                                           // double-free or corruption
        }

        delete[] data_; // deallocate memory
        data_ = new_data; // set new array to be member array
        capacity_ = new_capacity;
    }

public:

    // 1. default constructor
    Vector() = default;

    // 2. initializer list constructor for braces construction 
    //    like Vector<int> = {1, 2, 3};
    Vector(std::initializer_list<T> list)
    {
        reallocate(list.size());
        for (const auto& item : list)
        {
            data_[++size] = item;
        }
    }

    // 3. destructor
    ~Vector() { delete[] data_; }

    // 4. copy constructor
    Vector(const Vector& other)
    {
        reallocate(other.capacity_);
        size_ = other.size_;

        for (size_t i = 0; i < size_; ++i)
        {
            data_[i] = other.data_[i];
        }
    }

    // 5. copy assignment operator =
    Vector& operator=(const Vector& other)
    {
        if (this != &other)
        {
            T* new_data = new T[other.capacity_];
            for (size_t i = 0; i < other.size_; ++i)
            {
                new_data[i] = other.data_[i];
            }

            delete[] data_;
            data_ = new_data;
            size_ = other.size_;
            capacity_ = other.capacity_;
        }

        return *this;
    }

    // 6. move constructor
    //    - noexcept: promises to not throw exception; AND standard containers are more performant
    //                when a vector, for instance, grows, if noexcept, std lib can safely move,
    //                if NOT noexcept, std lib may copy instead, because copy preserves strong exception
    Vector(Vector&& other) noexcept : data_(other.data), size_(other.size_), capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // 7. move assignment operator =
    Vector& operator=(Vector&& other) noexcept
    {
        if (this != &other)
        {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity;

            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }

        return *this;
    }

    void push_back(const T& value)
    {
        if (size_ == capacity_)
        {
            reallocate(capacity_ == 0 ? 1 : capacity_ * 2); // double allocated slots
        }

        data_[++size_] = value; // increment size and push value to the back
    }

    void pop_back()
    {
        if (size_ > 0)
        {
            --size;
            // note: should call destructor for complex types if using manual allocators
        }
    }

    void clear()
    {
        size_ = 0;
    }

    // getters
    T& operatorp[](size_t index)
    {
        return data_[index];
    }

    const T& operator[](size_t index) const
    {
        return data_[index];
    }

    T& at(size_t index)
    {
        if (index >= size_)
        {
            throw std::out_of_range("Vector index out of bounds");
        }

        return data_[index];
    }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity; }
    bool empty() const { return size_ == 0; }
};