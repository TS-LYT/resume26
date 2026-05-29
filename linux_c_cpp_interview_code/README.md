# Linux C/C++ 嵌入式应用面试知识和代码演示

这套资料补充 Linux 嵌入式应用工程师面试常问点。每个目录都包含：

```text
README.md      面试说明、常见追问、排查思路
demo.c/.cpp    可运行代码演示，带详细中文注释
```

## 目录

```text
tcp_handshake_teardown      TCP 三次握手、四次挥手
c_language_stack_heap       C 语言栈、堆、static、全局变量
pointers_arrays_callbacks   指针、二级指针、数组指针、指针数组、回调函数
macro_bitfield              宏定义、条件编译、位操作、位域
socket_networking           socket、TCP server/client、网络编程基础
environment_programming     环境变量、命令行参数、工作目录
process_threading           多进程、多线程、fork、pthread
ipc_examples                进程通信：pipe、FIFO、共享内存
memory_leak_debug           内存泄漏排查、valgrind、asan
```

## 一次性编译

```bash
cd linux_c_cpp_interview_code
cmake -S . -B build
cmake --build build
```

部分示例是服务端或需要两个终端配合运行，README 里有单独说明。

## 面试学习建议

1. 先读每个模块的 README，练口述。
2. 再看代码注释，理解每一行和面试点的对应关系。
3. 最后自己改参数、改端口、改数组长度，再重新编译运行。
