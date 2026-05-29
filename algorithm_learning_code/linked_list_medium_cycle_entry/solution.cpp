#include <iostream>

struct ListNode {
    int val;
    ListNode* next;
};

static ListNode* detect_cycle(ListNode* head)
{
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            ListNode* p1 = head;
            ListNode* p2 = slow;

            while (p1 != p2) {
                p1 = p1->next;
                p2 = p2->next;
            }

            return p1;
        }
    }

    return nullptr;
}

int main()
{
    ListNode n1 {1, nullptr};
    ListNode n2 {2, nullptr};
    ListNode n3 {3, nullptr};
    ListNode n4 {4, nullptr};

    n1.next = &n2;
    n2.next = &n3;
    n3.next = &n4;
    n4.next = &n2;

    ListNode* entry = detect_cycle(&n1);
    if (entry != nullptr) {
        std::cout << "cycle entry=" << entry->val << "\n";
    } else {
        std::cout << "no cycle\n";
    }

    return 0;
}
