# 内存泄漏怎么排查

## 什么是内存泄漏

程序申请了堆内存，但不再使用后没有释放，导致进程占用内存持续增长。

## 排查思路

```text
1. 观察现象:
   top、htop、ps、/proc/<pid>/status 看 RSS 是否持续增长。

2. 找泄漏位置:
   valgrind --leak-check=full ./program
   或使用 AddressSanitizer。

3. 查代码模式:
   malloc 后是否所有路径都有 free。
   fopen 后是否 fclose。
   socket 后是否 close。
   new 后是否 delete。

4. 长时间运行验证:
   压测、循环请求、异常路径、断线重连。
```

## Valgrind 示例

```bash
cmake -S . -B build
cmake --build build
valgrind --leak-check=full ./build/memory_leak_demo
```

## ASan 示例

```bash
gcc -g -fsanitize=address memory_leak_demo.c -o leak_asan
ASAN_OPTIONS=detect_leaks=1 ./leak_asan
```

## 面试口述

我会先确认内存是否持续增长，再用 valgrind 或 ASan 定位泄漏栈。排查时重点看错误路径、提前 return、循环申请、容器持有指针、线程退出和 socket/fd 未关闭。
