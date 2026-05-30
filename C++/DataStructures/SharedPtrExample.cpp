#include <iostream>
#include <memory>

struct Child;

struct Parent
{
    std::shared_ptr<Child> child; // parent strictly owns child
    ~Parent() { std::cout << "Parent destroyed\n"; }
};

struct Child
{
    std::weak_ptr<Parent> parent; // child safely observes parent without owning
    ~Child() { std::cout << "Child destroyed\n"; }
};

int main()
{
    auto p = std::make_shared<Parent>();
    auto c = std::make_shared<Child>();

    p->child = c; // strong link
    c->parent = p; // weak link, breaks cycle

    // when main() ends, p and c go out of scope
    // both objects are automatically and properly destroyed
};

// notes: std::weak_ptr does not guarantee target object will remain alive, 
// so it cannot access the object's members directly
// in order to safely use the resource, you must temporarily upgrade it
// to a strong reference
// - use .lock(): call std::weak_ptr::lock
// - check result: if the underlying object was already destroyed, 
//   the returned pointer is nullptr
// - thread safety: this process is atomic, which ensures that the object 
//   will not be deleted midway through your access block even in 
//   multi-threaded environments