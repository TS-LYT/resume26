#include <stdio.h>

typedef struct ListNode {
    int val;
    struct ListNode* next;
} ListNode;

static ListNode* merge_two_lists(ListNode* l1, ListNode* l2)
{
    ListNode dummy;
    dummy.val = 0;
    dummy.next = NULL;

    ListNode* tail = &dummy;

    while (l1 != NULL && l2 != NULL) {
        if (l1->val <= l2->val) {
            tail->next = l1;
            l1 = l1->next;
        } else {
            tail->next = l2;
            l2 = l2->next;
        }

        tail = tail->next;
    }

    tail->next = (l1 != NULL) ? l1 : l2;
    return dummy.next;
}

static void print_list(ListNode* head)
{
    for (ListNode* cur = head; cur != NULL; cur = cur->next) {
        printf("%d -> ", cur->val);
    }
    printf("NULL\n");
}

int main(void)
{
    ListNode a3 = {5, NULL};
    ListNode a2 = {3, &a3};
    ListNode a1 = {1, &a2};

    ListNode b3 = {6, NULL};
    ListNode b2 = {4, &b3};
    ListNode b1 = {2, &b2};

    ListNode* merged = merge_two_lists(&a1, &b1);
    print_list(merged);

    return 0;
}
