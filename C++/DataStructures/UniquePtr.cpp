template <typename T>
class UniquePtr
{
private:
    T* ptr = nullptr; // underlying raw pointer

public:
    // 1. constructors & destructor
    explicit UniquePtr(T* p = nullptr) : ptr(p) {};

    ~UniquePtr() { delete ptr; }

    // 2. prevent copying (exclusive ownership)
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // 3. move semantics (transfer ownership)
    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr)
    {
        other.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr& other) noexcept
    {
        if (this != &other)
        {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // 4. pointer-like access
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; } // access without taking ownership
    
};