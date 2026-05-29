#include <stdio.h>
#include <stdlib.h>

typedef struct ListNode {
    int val;
    struct ListNode* next;
} ListNode;

static ListNode* create_node(int val)
{
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    if (node == NULL) {
        return NULL;
    }

    node->val = val;
    node->next = NULL;
    return node;
}

static void print_list(ListNode* head)
{
    for (ListNode* cur = head; cur != NULL; cur = cur->next) {
        printf("%d -> ", cur->val);
    }
    printf("NULL\n");
}

static ListNode* reverse_list(ListNode* head)
{
    ListNode* prev = NULL; // 已经反转好的部分。
    ListNode* cur = head;  // 当前要处理的节点。

    while (cur != NULL) {
        ListNode* next = cur->next; // 先保存后续节点，避免断链后丢失。
        cur->next = prev;           // 当前节点反向指向前一个节点。
        prev = cur;                 // prev 前进。
        cur = next;                 // cur 前进。
    }

    return prev; // prev 最后停在新的头节点。
}

static void free_list(ListNode* head)
{
    while (head != NULL) {
        ListNode* next = head->next;
        free(head);
        head = next;
    }
}

int main(void)
{
    ListNode* n1 = create_node(1);
    ListNode* n2 = create_node(2);
    ListNode* n3 = create_node(3);

    n1->next = n2;
    n2->next = n3;

    printf("before: ");
    print_list(n1);

    ListNode* new_head = reverse_list(n1);

    printf("after : ");
    print_list(new_head);

    free_list(new_head);
    return 0;
}
