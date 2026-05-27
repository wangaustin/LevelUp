#include <iostream>
#include <vector>
#include <stdexcept>

// heap: value of root node must be smallest among all its descendant nodes and 
// the same must be done for its left and right sub-trees too

// let node be stored at index i
// - left child: (i*2)+1
// - right cild: (i*2)+2
// - parent: (i-1)/2

class MinHeap
{
private:
    std::vector<int> heap;

    // move element upward until heap property is restored; 
    // used after insertion
    // basically: keep swapping the new element with its parent while it is smaller than parent
    // complexity: O(log n) because the heap height is log n
    void heapify_up(int index)
    {
        while (index > 0)
        {
            int parent = (index - 1) / 2;
            
            // parent <= child, heap property is valid
            if (heap[parent] <= heap[index]) break;

            // otherwise, swap to restore heap property
            std::swap(heap[parent], heap[index]);

            // continue checking upward
            index = parent;
        }
    }

    // move element downward until heap property is restored; 
    // used after removing minimum element
    // basically: keep swapping the current node with its smaller child while the child is smaller
    // complexity: O(log n) because the heap height is log n
    void heapify_down(int index)
    {
        int n = heap.size();

        while (true)
        {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int smallest = index;

            // check if left is smaller than current node
            if (left < n && heap[left] < heap[smallest])
            {
                smallest = left;
            }

            // check if right is smaller than current node
            if (right < n && heap[right] < heap[smallest])
            {
                smallest = right;
            }

            // if node is already smaller than both children, 
            // heap property is restored
            if (smallest == index) break;

            // otherwise, move smaller child up
            std::swap(heap[smallest], heap[index]);

            // continue heapifying down from child's position
            index = smallest;
        }
    }

public:

    // complexity: O(log n)
    void push(int value)
    {
        heap.push_back(value);

        heapify_up(heap.size() - 1);
    }

    // complexity: O(1)
    int top() const
    {
        if (heap.empty()) throw std::runtime_error("Heap is empty!");

        return heap[0];
    }

    // complexity: O(log n)
    void pop()
    {
        if (heap.empty())
        {
            throw std::runtime_error("Heap is empty!");
        }

        // move last element to root
        heap[0] = heap.back();

        // remove duplicate at last element
        heap.pop_back();

        // restore heap property if not empty
        if (!heap.empty())
        {
            heapify_down(0);
        }
    }

    bool empty() const
    {
        return heap.empty();
    }

    int size() const
    {
        return heap.size();
    }

    void print() const
    {
        for (int x : heap)
        {
            std::cout << x << " ";
        }

        std::cout << std::endl;
    }
};

int main()
{
    MinHeap h;

    h.push(10);
    h.push(4);
    h.push(15);
    h.push(2);
    h.push(8);

    h.print();

    std::cout << "Min: " << h.top() << "\n";

    h.pop();

    std::cout << "After .pop(): \n";

    h.print();

    return 0;
}