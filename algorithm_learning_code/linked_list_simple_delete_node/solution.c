#include <stdio.h>
#include <stdlib.h>

typedef struct ListNode {
    int val;
    struct ListNode* next;
} ListNode;

static ListNode* create_node(int val)
{
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    node->val = val;
    node->next = NULL;
    return node;
}

static ListNode* remove_elements(ListNode* head, int val)
{
    ListNode dummy;
    dummy.val = 0;
    dummy.next = head;

    ListNode* prev = &dummy;
    while (prev->next != NULL) {
        if (prev->next->val == val) {
            ListNode* to_delete = prev->next;
            prev->next = to_delete->next;
            free(to_delete);
        } else {
            prev = prev->next;
        }
    }

    return dummy.next;
}

static void print_list(ListNode* head)
{
    for (ListNode* cur = head; cur != NULL; cur = cur->next) {
        printf("%d -> ", cur->val);
    }
    printf("NULL\n");
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
    ListNode* n3 = create_node(6);
    ListNode* n4 = create_node(3);
    ListNode* n5 = create_node(6);
    n1->next = n2;
    n2->next = n3;
    n3->next = n4;
    n4->next = n5;

    ListNode* head = remove_elements(n1, 6);
    print_list(head);
    free_list(head);

    return 0;
}
