#include <iostream>

struct ListNode {
    int val;
    ListNode* next;
};

static ListNode* merge_two_lists(ListNode* l1, ListNode* l2)
{
    ListNode dummy {0, nullptr};
    ListNode* tail = &dummy;

    while (l1 != nullptr && l2 != nullptr) {
        if (l1->val <= l2->val) {
            tail->next = l1;
            l1 = l1->next;
        } else {
            tail->next = l2;
            l2 = l2->next;
        }

        tail = tail->next;
    }

    tail->next = (l1 != nullptr) ? l1 : l2;
    return dummy.next;
}

static void print_list(ListNode* head)
{
    for (ListNode* cur = head; cur != nullptr; cur = cur->next) {
        std::cout << cur->val << " -> ";
    }
    std::cout << "NULL\n";
}

int main()
{
    ListNode a3 {5, nullptr};
    ListNode a2 {3, &a3};
    ListNode a1 {1, &a2};

    ListNode b3 {6, nullptr};
    ListNode b2 {4, &b3};
    ListNode b1 {2, &b2};

    print_list(merge_two_lists(&a1, &b1));
    return 0;
}
