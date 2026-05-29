# 第 6 阶段：deque 双端队列

## 概念

`deque` 是双端队列，支持头部和尾部快速插入删除。它通常是分段连续内存，不是 vector 那样的一整块连续内存。

## 编译运行

```bash
cmake -S . -B build
cmake --build build
./build/demo
```

## 本示例覆盖

`push_back`、`push_front`、`pop_back`、`pop_front`、`front`、`back`、`[]`、`size`、`empty`，以及双端优先缓存。

## 常见错误

1. 以为 deque 底层完全连续。
2. 只在尾部追加时盲目用 deque，vector 可能更简单更快。
3. 空 deque 时调用 `front()` 或 `back()`。

## 嵌入式注意

当头尾都需要频繁插入删除时，deque 比 vector 更合适。若数据主要顺序存储和随机访问，优先考虑 vector 或 array。

## 面试回答

deque 支持头尾快速操作，通常是分段连续结构。queue 默认底层常用 deque，因为它很好支持队尾插入和队头删除。
