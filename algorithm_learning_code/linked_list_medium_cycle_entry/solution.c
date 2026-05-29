#include <stdio.h>

typedef struct ListNode {
    int val;
    struct ListNode* next;
} ListNode;

static ListNode* detect_cycle(ListNode* head)
{
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            // 第一次相遇，说明有环。
            ListNode* p1 = head;
            ListNode* p2 = slow;

            // p1 从头走，p2 从相遇点走，再次相遇就是入口。
            while (p1 != p2) {
                p1 = p1->next;
                p2 = p2->next;
            }

            return p1;
        }
    }

    return NULL;
}

int main(void)
{
    // 为了简单演示，节点放在栈上，不需要 malloc/free。
    ListNode n1 = {1, NULL};
    ListNode n2 = {2, NULL};
    ListNode n3 = {3, NULL};
    ListNode n4 = {4, NULL};

    n1.next = &n2;
    n2.next = &n3;
    n3.next = &n4;
    n4.next = &n2; // 制造环，入口是 n2。

    ListNode* entry = detect_cycle(&n1);
    if (entry != NULL) {
        printf("cycle entry=%d\n", entry->val);
    } else {
        printf("no cycle\n");
    }

    // 注意：有环链表不能直接用普通 free_list 释放，否则会死循环。
    return 0;
}
