# 第 3 阶段：list 双向链表

## 概念

`list` 底层是双向链表，节点内存不连续，每个节点有前后指针。

## 编译运行

```bash
cmake -S . -B build
cmake --build build
./build/demo
```

## 本示例覆盖

`push_back`、`push_front`、`insert`、`erase`、`remove_if`、`empty`、`size`、遍历、模拟节点上线下线。

## 常见错误

1. 试图用 `list[0]` 访问元素。
2. `erase` 后继续使用已经失效的迭代器。
3. 以为 list 一定比 vector 快。

## 嵌入式注意

list 每个节点都有额外指针和单独分配开销，缓存局部性也差。除非频繁在中间插入删除，否则实际工程里 vector 往往更常用。

## 面试回答

list 是双向链表，不支持随机访问；在已找到位置时插入删除快，但内存开销和缓存命中率不如 vector。
