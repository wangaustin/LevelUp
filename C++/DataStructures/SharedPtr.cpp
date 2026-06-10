#include <atomic>
#include <cstddef>
#include <utility>

template <typename T>
class SharedPtr
{
private:
    struct ControlBlock
    {
        std::atomic<std::size_t> count;

        // starts ownership count at one
        ControlBlock() : count(1) {}
    };

    T* ptr_;
    ControlBlock* ctrl_;

    // decrement reference count and deletes resource if this was the last owner
    void release()
    {
        if (!ctrl_) return;
        
        if (ctrl_->count.fetch_sub(1, std::memory_order_acq_rel) == 1)
        // decrement may delete the object, acq_rel ensures that when final owner deletes object, 
        // prior operations related to ownership are properly ordered before destruction
        {
            delete ptr_;
            delete ctrl_;
        }

        ptr_ = nullptr;
        ctrl_ = nullptr;
    }

public:

    // creates empty shared ptr
    SharedPtr() : ptr_(nullptr), ctrl_(nullptr){}

    // takes ownership of raw pointer
    explicit SharedPtr(T* ptr)
        : ptr_(ptr),
          ctrl_(ptr ? new ControlBlock() : nullptr) {}

    SharedPtr(const SharedPtr& other)
        : ptr_(other.ptr_),
          ctrl_(other.ctrl_)
    {
        if (ctrl_)
        {
            ctrl_->count.fetch_add(1, std::memory_order_relaxed); // increment does not need to synchronize, just need to be numerically correct
        }
    }

    // moves ownership without changing reference count
    SharedPtr(SharedPtr&& other) noexcept
        : ptr_(other.ptr_),
          ctrl_(other.ctrl_)
    {
        other.ptr_ = nullptr;
        other.ctrl_ = nullptr;
    }

    // release this owner's reference
    ~SharedPtr()
    {
        release();
    }

    SharedPtr& operator=(const SharedPtr& other)
    {
        if (this == &other) return *this;

        release();

        ptr_ = other.ptr_;
        ctrl_ = other.ctrl_;

        if (ctrl_)
        {
            ctrl_->count.fetch_add(1, std::memory_order_relaxed);
        }

        return *this;
    }

    // move-assigns ownership without changing reference count
    SharedPtr& operator=(SharedPtr&& other) noexcept
    {
        if (this == &other) return *this;

        release();

        ptr_ = other.ptr_;
        ctrl_ = other.ctrl_;

        other.ptr_ = nullptr;
        other.ctrl = nullptr;

        return *this;
    }

    // get raw pointer without transferring ownership
    T* get() const
    {
        return ptr_;
    }

    // dereferences managed object
    T& operator*() const
    {
        return *ptr_;
    }

    // accesses member of managed object
    T* operator->() const
    {
        return ptr_;
    }

    // returns current reference count
    std::size_t use_count() const
    {
        return ctrl_ ? ctrl_->count.load(std::memory_order_relaxed) : 0;
    }

    // returns true if this shared ptr owns an object
    explicit operator bool() const
    {
        return ptr_ != nullptr;
    }

    // release current ownership and optionally takes ownership of a new pointer
    void reset(T* ptr = nullptr)
    {
        release();

        ptr_ = ptr;
        ctrl_ = ptr ? new ControlBlock() : nullptr;
    }

    void swap(SharedPtr& other) noexcept
    {
        std::swap(ptr_, other.ptr_);
        std::swap(ctrl_, other.ctrl_);
    }
};