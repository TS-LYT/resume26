# 鸡哥算法学习代码：数组、链表、队列、栈

这套代码偏面试基础训练，按“简单 + 中等”难度组织。每道题都有：

```text
1. problem.md      题目、思路、复杂度、面试口述
2. solution.c      C 语言实现
3. solution.cpp    C++ 实现
```

## 题目目录

```text
array_simple_move_zeroes              数组简单：移动零
array_simple_binary_search            数组简单：二分查找
array_medium_min_subarray_len         数组中等：长度最小的子数组
array_medium_rotate_array             数组中等：旋转数组
linked_list_simple_reverse            链表简单：反转链表
linked_list_simple_delete_node        链表简单：删除指定值节点
linked_list_medium_cycle_entry        链表中等：环形链表入口
linked_list_medium_merge_two_lists    链表中等：合并两个有序链表
queue_simple_circular_queue           队列简单：循环队列
queue_simple_queue_using_stacks       队列简单：用两个栈实现队列
queue_medium_sliding_window_max       队列中等：滑动窗口最大值
queue_medium_bfs_grid                 队列中等：网格 BFS 最短路
stack_simple_valid_parentheses        栈简单：有效括号
stack_simple_min_stack                栈简单：最小栈
stack_medium_daily_temperatures       栈中等：每日温度
stack_medium_eval_rpn                 栈中等：逆波兰表达式求值
```

## 一次性编译全部

```bash
cd algorithm_learning_code
cmake -S . -B build
cmake --build build
```

## 运行示例

```bash
./build/array_simple_move_zeroes_c
./build/array_simple_move_zeroes_cpp
./build/linked_list_simple_reverse_c
./build/linked_list_simple_reverse_cpp
```

## 学习建议

1. 先读 `problem.md`，确认题意。
2. 先看 C 版本，理解数组、指针、结构体。
3. 再看 C++ 版本，理解 `vector`、`string`、`queue`、`stack` 等写法。
4. 最后自己遮住代码，尝试重新写一遍。
