# 第 5 阶段：stack 栈

## 概念

`stack` 是后进先出 LIFO 容器适配器，最后压入的元素最先弹出。

## 编译运行

```bash
cmake -S . -B build
cmake --build build
./build/demo
```

## 本示例覆盖

`push`、`pop`、`top`、`empty`、`size`、反向输出、括号匹配、函数调用栈思想。

## 常见错误

1. 空栈时调用 `top()`。
2. 以为 `pop()` 会返回元素。
3. 需要访问中间元素时误用 stack。

## 嵌入式注意

stack 适合状态回退、解析、匹配类问题。不要和程序运行时的线程栈混淆，STL `stack` 默认底层通常仍会使用动态内存。

## 面试回答

stack 是 LIFO，常用于括号匹配、表达式解析、撤销操作、函数调用栈思想。访问栈顶用 `top()`，删除栈顶用 `pop()`。
