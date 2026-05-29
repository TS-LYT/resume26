#include <iostream>
#include <memory>

struct ListNode {
    int val;
    ListNode* next;

    explicit ListNode(int value)
        : val(value), next(nullptr)
    {
    }
};

static void print_list(ListNode* head)
{
    for (ListNode* cur = head; cur != nullptr; cur = cur->next) {
        std::cout << cur->val << " -> ";
    }
    std::cout << "NULL\n";
}

static ListNode* reverse_list(ListNode* head)
{
    ListNode* prev = nullptr;
    ListNode* cur = head;

    while (cur != nullptr) {
        ListNode* next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }

    return prev;
}

int main()
{
    // 这里为了贴近面试题，链表节点仍然用裸指针连接。
    // unique_ptr 负责释放内存，避免示例里出现内存泄漏。
    auto n1 = std::make_unique<ListNode>(1);
    auto n2 = std::make_unique<ListNode>(2);
    auto n3 = std::make_unique<ListNode>(3);

    n1->next = n2.get();
    n2->next = n3.get();

    std::cout << "before: ";
    print_list(n1.get());

    ListNode* new_head = reverse_list(n1.get());

    std::cout << "after : ";
    print_list(new_head);

    return 0;
}
