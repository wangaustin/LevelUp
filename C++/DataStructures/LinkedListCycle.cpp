// floyd's cycle detection
// slow / fast pointer

struct ListNode
{
    int val;
    ListNode* next;

    ListNode(int v) : val(v), next(nullptr) {}
};

bool hasCycle(ListNode* head)
{
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast != nullptr && fast->next != nullptr)
    {
        slow = slow->next; // move 1
        fast = fast->next->next; // move 2

        if (slow == fast)
        {
            return true;
        }

        return false;
    }
}
