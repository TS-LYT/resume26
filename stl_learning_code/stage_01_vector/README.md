# 第 1 阶段：vector 动态数组

## 概念

`vector` 类似 C 语言动态数组，底层是连续内存，支持 `[]` 随机访问。尾部 `push_back` 很常用，但容量不足时会扩容。

## 编译运行

```bash
cmake -S . -B build
cmake --build build
./build/demo
```

## 本示例覆盖

`push_back`、`pop_back`、`size`、`capacity`、`empty`、`clear`、`reserve`、`resize`、`[]`、`at()`、范围 for 遍历、扩容观察。

## 常见错误

1. 把 `reserve` 当成 `resize`：`reserve` 不会创建元素。
2. `pop_back` 前不判断 `empty`。
3. 保存了元素指针后继续 `push_back`，扩容后旧指针可能失效。

## 嵌入式注意

实时路径中尽量提前 `reserve`，避免运行时扩容造成不可控耗时和内存碎片。

## 面试回答

vector 底层是动态数组，内存连续，随机访问快。容量不足时会重新申请更大内存并移动旧元素，扩容后旧指针、引用、迭代器可能失效。
