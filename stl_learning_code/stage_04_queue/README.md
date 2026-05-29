# 第 4 阶段：queue 队列

## 概念

`queue` 是先进先出 FIFO 容器适配器，适合任务队列、消息队列、网络数据包队列。

## 编译运行

```bash
cmake -S . -B build
cmake --build build
./build/demo
```

## 本示例覆盖

`push`、`pop`、`front`、`back`、`empty`、`size`，以及网络包接收和处理流程。

## 常见错误

1. 以为 `pop()` 会返回元素。
2. 空队列时调用 `front()`。
3. 需要遍历全部元素时误用 queue，queue 不适合随机遍历。

## 嵌入式注意

queue 常用于生产者消费者模型。多线程场景需要互斥锁、条件变量等同步机制，标准 `queue` 自身不是线程安全队列。

## 面试回答

queue 是 FIFO，先进入的元素先处理。`front()` 获取队头，`pop()` 删除队头，通常先 `front()` 再 `pop()`。
