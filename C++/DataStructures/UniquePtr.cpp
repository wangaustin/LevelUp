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
    // note: double ampersands bind to rvalues only, so the function does not
    //       silently destroys a named object the caller still thinks is valid
    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr)
    {
        other.ptr = nullptr;
    }
    /**
     * Example of what could go wrong if you used Unique& instead of UniquePtr&&
     * if you take a lvalue reference, when you do other.ptr = nullptr,
     * you're silently stealing data from a named object
     * 
     * void process_data(UniquePtr p) { // do something with pointer }
     * 
     * int main()
     * {
     *      UniquePtr original(new int(42));
     * 
     *      // you want to pass 'original' to a function
     *      // and because it takes a regular reference, this compiles,
     *      // BUT!!!
     *      process_data(original);
     * 
     *      // CRASH!!! 'original' was silently emptied inside constructor
     *      // original is now nullptr, and you get undefined behavior
     *      std::cout << *original << std::endl;
     * }
     * 
     * 
     */

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

    // 5. Factory function
    // note: variadic template that perfect-forwards any arguments to T's constructor
    template <typename T, typename... Args>
    UniquePtr<T> MakeUniquePtr(Args&&... args)
    {
        return UniquePtr<T>(new T(std::forward<Args>(args)...));
    }
    
};