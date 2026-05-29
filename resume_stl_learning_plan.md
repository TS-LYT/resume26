# Linux 嵌入式应用工程师 STL 学习计划

这份文档用于给 Codex 作为学习任务说明，帮助我围绕 **Linux 嵌入式应用工程师面试准备** 学习 C++ STL 中常用的数组、链表、队列类容器，并通过代码练习逐步掌握。

---

## 1. 我的基础情况

我正在准备 **Linux 嵌入式应用工程师** 面试。

目前基础：

```text
1. 主要语言：C / C++
2. C 比较熟练
3. C++ 基础较弱，可以当作初学者
4. 能简单使用 Python
5. 有 Linux 应用开发经验
6. 做过 socket、HTTP、Mongoose、网络编程
7. 做过多线程、多进程、环境编程
8. 现在希望重点补 C++ STL 中常用容器的代码能力
```

学习目标不是刷复杂算法，而是：

```text
1. 能看懂 C++ 工程代码
2. 能写出基本 STL 容器代码
3. 能回答面试常见问题
4. 能理解 STL 在嵌入式 Linux 中的使用注意事项
5. 能把 C 语言经验迁移到 C++ 容器使用上
```

---

## 2. STL 学习范围

暂时不需要完整学习 STL，只重点学习下面这些容器：

```text
1. vector   动态数组
2. array    固定数组
3. list     双向链表
4. queue    队列
5. stack    栈
6. deque    双端队列
```

优先级：

```text
必须掌握：vector、list、queue、stack
建议掌握：array
了解掌握：deque
```

---

## 3. 学习顺序

建议按照下面顺序学习：

```text
第 1 阶段：vector
第 2 阶段：array
第 3 阶段：list
第 4 阶段：queue
第 5 阶段：stack
第 6 阶段：deque
第 7 阶段：综合项目练习
第 8 阶段：面试题总结
```

每个阶段都需要包括：

```text
1. 基本概念
2. 常用接口
3. 最小可运行代码
4. CMakeLists.txt
5. 代码注释
6. 编译运行命令
7. 常见错误
8. 面试回答
9. 嵌入式使用注意事项
```

统一编译方式：

```bash
cmake -S . -B build
cmake --build build
./build/demo
```

---

## 4. 第 1 阶段：vector 动态数组

### 4.1 需要学习的内容

```text
1. vector 是什么
2. vector 和 C 数组的区别
3. push_back
4. pop_back
5. size
6. capacity
7. empty
8. clear
9. reserve
10. resize
11. 下标访问 []
12. at()
13. 遍历方式
14. 迭代器基本使用
15. vector 扩容机制
16. vector 迭代器失效
17. 嵌入式中使用 vector 的注意事项
```

### 4.2 练习代码任务

```text
1. 用 vector 保存整数并打印
2. 用 vector 保存传感器采样值
3. 计算最大值、最小值、平均值
4. 观察 push_back 时 size 和 capacity 的变化
5. 使用 reserve 减少扩容
6. 删除最后一个元素
7. 修改指定下标的元素
```

### 4.3 面试重点

```text
1. vector 底层是什么？
2. vector 的 size 和 capacity 区别？
3. reserve 和 resize 区别？
4. vector 扩容时发生什么？
5. vector 什么时候迭代器失效？
6. 嵌入式中为什么要小心 vector 动态扩容？
```

### 4.4 需要掌握的核心回答

vector 底层是动态数组，内存连续，支持 O(1) 随机访问。尾部插入通常效率较高，但如果容量不足会触发扩容，扩容时会重新申请更大的连续内存，并把旧元素拷贝或移动过去。扩容后原来的指针、引用、迭代器可能失效。嵌入式实时性要求高的场景中，应尽量提前使用 `reserve()` 分配空间，减少运行时动态扩容。

---

## 5. 第 2 阶段：array 固定数组

### 5.1 需要学习的内容

```text
1. array 是什么
2. array 和普通 C 数组区别
3. array 和 vector 区别
4. size
5. at
6. []
7. front
8. back
9. 遍历
```

### 5.2 练习代码任务

```text
1. 用 array 保存固定数量的传感器数据
2. 遍历打印 array
3. 修改 array 中的某个元素
4. 计算 array 中数据的平均值
```

### 5.3 面试重点

```text
1. array 是固定大小还是动态大小？
2. array 和 vector 的区别？
3. 嵌入式场景什么时候适合用 array？
```

### 5.4 需要掌握的核心回答

array 是 C++ 标准库封装的固定大小数组，大小在编译期确定，不能动态扩容。它比普通 C 数组更安全、更方便，因为提供了 `size()`、`at()`、`front()`、`back()` 等接口。数据数量固定、实时性要求高、希望避免动态内存分配时，嵌入式场景可以优先考虑 `array`。

---

## 6. 第 3 阶段：list 双向链表

### 6.1 需要学习的内容

```text
1. list 是什么
2. list 底层结构
3. push_back
4. push_front
5. pop_back
6. pop_front
7. insert
8. erase
9. remove
10. clear
11. empty
12. size
13. 遍历
14. 为什么 list 不能用下标访问
15. list 和 vector 的区别
```

### 6.2 练习代码任务

```text
1. 用 list 保存节点 ID
2. 在头部插入节点
3. 在尾部插入节点
4. 删除指定节点
5. 遍历打印所有节点
6. 模拟设备节点上线和下线
```

### 6.3 面试重点

```text
1. list 底层是什么？
2. list 为什么不能随机访问？
3. list 插入删除为什么快？
4. list 和 vector 的区别？
5. 为什么实际工程中 vector 通常比 list 更常用？
```

### 6.4 需要掌握的核心回答

list 底层是双向链表，节点之间通过前后指针连接，内存不连续。它不支持下标随机访问，访问某个位置需要从头或尾遍历。但在已经找到位置的情况下，插入和删除效率很高，通常是 O(1)。不过由于每个节点都有额外指针开销，且缓存命中率不如 vector，所以实际工程中 vector 往往更常用。

---

## 7. 第 4 阶段：queue 队列

### 7.1 需要学习的内容

```text
1. queue 是什么
2. queue 的先进先出 FIFO
3. push
4. pop
5. front
6. back
7. empty
8. size
9. pop 为什么不返回元素
```

### 7.2 练习代码任务

```text
1. 用 queue 模拟消息队列
2. 用 queue 保存网络数据包
3. 每次从队头取一个包处理
4. 处理完后 pop 删除
```

建议使用结构体：

```cpp
struct Packet
{
    int id;
    int len;
    std::string data;
};
```

### 7.3 面试重点

```text
1. queue 的特点是什么？
2. queue 和 stack 的区别？
3. queue 的 pop 为什么不能直接返回元素？
4. 嵌入式中 queue 可以用来做什么？
```

### 7.4 需要掌握的核心回答

queue 是先进先出 FIFO 容器适配器，适合模拟消息队列、任务队列、网络数据包缓存等场景。`front()` 访问队头元素，`pop()` 删除队头元素。注意 `pop()` 不返回元素，所以通常需要先 `front()` 取出数据，再调用 `pop()` 删除。

---

## 8. 第 5 阶段：stack 栈

### 8.1 需要学习的内容

```text
1. stack 是什么
2. stack 的后进先出 LIFO
3. push
4. pop
5. top
6. empty
7. size
```

### 8.2 练习代码任务

```text
1. 用 stack 保存数据并反向输出
2. 用 stack 判断括号是否匹配
3. 用 stack 模拟函数调用栈的基本思想
```

### 8.3 面试重点

```text
1. stack 的特点是什么？
2. stack 和 queue 的区别？
3. stack 常用于什么场景？
```

### 8.4 需要掌握的核心回答

stack 是后进先出 LIFO 容器适配器，最后放进去的元素最先被取出。`top()` 获取栈顶元素，`pop()` 删除栈顶元素。stack 常用于括号匹配、表达式解析、撤销操作、函数调用栈思想等场景。

---

## 9. 第 6 阶段：deque 双端队列

### 9.1 需要学习的内容

```text
1. deque 是什么
2. deque 和 vector 的区别
3. deque 和 queue 的区别
4. push_back
5. push_front
6. pop_back
7. pop_front
8. front
9. back
10. []
11. size
12. empty
```

### 9.2 练习代码任务

```text
1. 用 deque 实现一个双端缓存
2. 普通数据从尾部插入
3. 高优先级数据从头部插入
4. 每次从头部取数据处理
```

### 9.3 面试重点

```text
1. deque 底层是不是完全连续内存？
2. deque 和 vector 的区别？
3. 什么场景用 deque？
4. queue 默认底层为什么常用 deque？
```

### 9.4 需要掌握的核心回答

deque 是双端队列，支持头部和尾部快速插入删除。它不像 vector 那样要求整体连续内存，通常是分段连续结构。vector 更适合尾部插入和随机访问，deque 更适合头尾都需要频繁插入删除的场景。queue 默认底层常用 deque，因为 deque 能很好地支持队尾插入和队头删除。

---

## 10. 综合练习项目

每个项目都要求：

```text
1. 有完整 main.cpp
2. 有 CMakeLists.txt
3. 有详细中文注释
4. 有编译命令
5. 有运行结果示例
6. 先做最小可运行版本，再逐步扩展
```

---

### 10.1 项目 1：传感器采样缓存

要求：

```text
1. 使用 vector 保存传感器采样值
2. 支持添加采样值
3. 支持打印所有采样值
4. 支持计算最大值
5. 支持计算最小值
6. 支持计算平均值
7. 使用 reserve 提前分配空间
```

---

### 10.2 项目 2：设备节点管理

要求：

```text
1. 使用 list 保存设备节点
2. 节点包含 nodeId、name、online 状态
3. 支持添加节点
4. 支持删除节点
5. 支持修改节点在线状态
6. 支持遍历打印所有节点
```

结构体参考：

```cpp
struct Node
{
    int nodeId;
    std::string name;
    bool online;
};
```

---

### 10.3 项目 3：网络数据包队列

要求：

```text
1. 使用 queue 保存网络数据包
2. 数据包包含 id、长度、内容
3. 模拟接收数据包
4. 模拟处理数据包
5. 每次从队头取出一个数据包
6. 处理完后从队列删除
```

结构体参考：

```cpp
struct Packet
{
    int id;
    int len;
    std::string data;
};
```

---

### 10.4 项目 4：双端优先缓存

要求：

```text
1. 使用 deque 保存任务
2. 普通任务从队尾插入
3. 紧急任务从队头插入
4. 每次从队头取任务处理
5. 模拟嵌入式任务调度场景
```

结构体参考：

```cpp
struct Task
{
    int id;
    std::string name;
    bool urgent;
};
```

---

### 10.5 项目 5：括号匹配检查

要求：

```text
1. 使用 stack
2. 输入一个字符串
3. 判断括号是否匹配
4. 支持 () [] {}
```

---

## 11. 代码要求

Codex 生成代码时需要遵守：

```text
1. 使用 C++11 或 C++17
2. 代码尽量简单，不要写复杂模板
3. 每个示例都要有详细中文注释
4. 每个项目都要给出 main.cpp
5. 每个项目都要给出 CMakeLists.txt
6. 每个项目都要给出编译命令
7. 每个项目都要给出运行结果示例
8. 不要一次写太复杂，先写最小可运行版本
9. 先保证能编译运行，再逐步扩展
```

统一 CMakeLists.txt 示例：

```cmake
cmake_minimum_required(VERSION 3.10)
project(stl_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable/demo main.cpp)
```

注意：上面 `add_executable` 应写为：

```cmake
add_executable(demo main.cpp)
```

---

## 12. 教学方式要求

Codex 教学时请按照下面方式：

```text
1. 先解释这个容器是什么
2. 再类比 C 语言中的结构
3. 再给最小代码
4. 再解释每一行代码
5. 再给一个小练习
6. 再指出常见错误
7. 最后总结面试回答
```

例如讲 vector 时，不要只给代码，还要说明：

```text
1. vector 类似 C 语言动态数组
2. 底层是连续内存
3. push_back 可能触发扩容
4. 扩容后原来的指针和迭代器可能失效
5. 嵌入式中实时性要求高时要提前 reserve
```

---

## 13. 面试题总结范围

最后需要整理下面这些面试题答案：

```text
1. vector 底层是什么？
2. vector 和普通数组区别？
3. vector 和 array 区别？
4. vector 和 list 区别？
5. list 底层是什么？
6. queue 和 stack 区别？
7. deque 和 vector 区别？
8. reserve 和 resize 区别？
9. size 和 capacity 区别？
10. vector 扩容会发生什么？
11. vector 迭代器什么时候失效？
12. 嵌入式中使用 STL 要注意什么？
```

回答风格要求：

```text
1. 简洁
2. 适合面试口述
3. 不要太学术
4. 能结合 Linux 嵌入式应用开发
```

---

## 14. 最终目标

通过这套学习，希望达到：

```text
1. 能熟练使用 vector、array、list、queue、stack、deque
2. 能看懂项目里简单的 C++ STL 代码
3. 能写出基本的容器操作代码
4. 能理解容器底层原理
5. 能回答 C++ STL 基础面试题
6. 能知道嵌入式场景中 STL 的风险和使用方法
```

---

## 15. 给 Codex 的执行提示词

可以直接把下面这段发给 Codex：

```text
请根据这份文档，从第 1 阶段 vector 开始教我。每次只生成一个小项目，不要一次生成全部内容。每次代码都必须包含 main.cpp、CMakeLists.txt、中文注释、编译命令和运行示例，并且必须能直接用 cmake 编译运行。我的 C++ 基础较弱，请用适合初学者和 Linux 嵌入式应用工程师面试的方式讲解。
```
