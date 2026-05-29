#include <iostream>

struct ListNode {
    int val;
    ListNode* next;
};

static ListNode* remove_elements(ListNode* head, int val)
{
    ListNode dummy {0, head};
    ListNode* prev = &dummy;

    while (prev->next != nullptr) {
        if (prev->next->val == val) {
            prev->next = prev->next->next;
        } else {
            prev = prev->next;
        }
    }

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
    ListNode n5 {6, nullptr};
    ListNode n4 {3, &n5};
    ListNode n3 {6, &n4};
    ListNode n2 {2, &n3};
    ListNode n1 {1, &n2};

    ListNode* head = remove_elements(&n1, 6);
    print_list(head);

    return 0;
}
