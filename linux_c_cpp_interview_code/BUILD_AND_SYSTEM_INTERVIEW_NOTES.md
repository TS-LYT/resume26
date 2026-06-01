# CMake、AutoTools 和 Linux 系统编程面试笔记

这份文档用于复习 Linux C/C++ 嵌入式应用面试中常见的构建工具、进程线程、内存排查、socket、网络编程、回调函数和进程通信问题。重点不是背概念，而是能在面试里把“是什么、怎么用、为什么、怎么排查”讲清楚。

## 一、CMake

### 1. CMake 是什么

CMake 是一个跨平台构建系统生成工具。它本身通常不直接编译源码，而是根据 `CMakeLists.txt` 生成实际构建系统文件，例如 Unix Makefile、Ninja 构建文件、Visual Studio 工程等。

常见流程：

```bash
cmake -S . -B build
cmake --build build
```

含义：

- `-S .`：源码目录是当前目录。
- `-B build`：构建目录是 `build`。
- `cmake --build build`：调用实际构建工具进行编译。

### 2. CMake 的核心概念

常见命令：

```cmake
cmake_minimum_required(VERSION 3.10)
project(demo C)

add_executable(app main.c)
target_include_directories(app PRIVATE include)
target_link_libraries(app PRIVATE pthread)
```

核心概念：

- `project()`：定义工程名称和语言。
- `add_executable()`：生成可执行程序。
- `add_library()`：生成库。
- `target_include_directories()`：给某个目标添加头文件搜索路径。
- `target_link_libraries()`：给某个目标链接库。
- `target_compile_options()`：给某个目标添加编译选项。
- `target_compile_definitions()`：给某个目标添加宏定义。

现代 CMake 更推荐围绕 target 写配置，而不是大量使用全局变量。因为 target 方式更清晰，也更容易控制依赖关系。

### 3. Debug 和 Release

单配置生成器，例如 Makefile、Ninja：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

常见类型：

- `Debug`：带调试信息，优化较少，适合 gdb 调试。
- `Release`：优化更多，适合发布。
- `RelWithDebInfo`：既优化又保留调试信息。

### 4. 面试常问

问题：CMake 和 Makefile 有什么区别？

回答：Makefile 是具体构建规则，直接告诉 make 怎么编译；CMake 是更上层的构建配置工具，用 `CMakeLists.txt` 生成 Makefile 或 Ninja 文件。CMake 跨平台能力更强，也更适合管理多目录、多库、多目标工程。

问题：为什么推荐 out-of-source build？

回答：也就是把构建产物放到 `build` 目录，不污染源码目录。这样可以同时维护 Debug、Release 多套构建目录，也方便删除构建产物。

问题：`target_link_libraries(app PRIVATE pthread)` 里的 `PRIVATE` 是什么意思？

回答：

- `PRIVATE`：只给当前目标使用。
- `PUBLIC`：当前目标使用，依赖它的目标也会继承。
- `INTERFACE`：当前目标自己不用，只传递给依赖它的目标。

## 二、AutoTools

### 1. AutoTools 是什么

AutoTools 是 GNU 体系下传统的构建工具链，常见组件包括：

- `autoconf`：根据 `configure.ac` 生成 `configure` 脚本。
- `automake`：根据 `Makefile.am` 生成 `Makefile.in`。
- `libtool`：处理跨平台动态库、静态库构建差异。

用户最终常见使用方式：

```bash
./configure
make
make install
```

### 2. AutoTools 的典型生成流程

维护者生成构建脚本：

```bash
autoreconf -i
```

使用者编译安装：

```bash
./configure --prefix=/usr/local
make
make install
```

大致关系：

```text
configure.ac + Makefile.am
        |
        v
autoreconf -i
        |
        v
configure + Makefile.in
        |
        v
./configure
        |
        v
Makefile
        |
        v
make
```

### 3. AutoTools 的特点

优点：

- 在 Unix/Linux 生态中历史悠久。
- `./configure && make && make install` 非常通用。
- 很适合做环境检测，例如检查头文件、库函数、编译器特性。

缺点：

- 文件较多，学习曲线陡。
- 宏和脚本可读性一般。
- 对新项目来说，很多团队更倾向使用 CMake 或 Meson。

### 4. CMake 和 AutoTools 对比

```text
CMake:
  跨平台能力强，适合现代 C/C++ 工程，多 IDE 支持好。

AutoTools:
  GNU/Linux 传统项目常见，configure 检测能力强，源码包发布常见。
```

面试口述：

我理解 CMake 更像现代跨平台工程配置工具，通过 `CMakeLists.txt` 生成具体构建系统；AutoTools 是 GNU 传统构建体系，最终给用户提供 `./configure && make && make install` 的安装方式。维护老项目时经常遇到 AutoTools，新项目一般更常用 CMake。

## 三、多进程

### 1. 什么是进程

进程是操作系统资源分配的基本单位。每个进程有独立的虚拟地址空间、文件描述符表、信号处理方式、进程 ID 等资源。

创建进程常见方式：

- `fork()`：复制当前进程，生成子进程。
- `exec()`：用新程序替换当前进程映像。
- `system()`：简单执行 shell 命令，内部通常会创建子进程。

### 2. fork 后父子进程有什么关系

`fork()` 后父子进程从同一位置继续执行，但返回值不同：

- 父进程中返回子进程 PID。
- 子进程中返回 0。
- 失败返回 -1。

父子进程地址空间逻辑上独立。Linux 使用写时复制，fork 后并不会立刻复制所有内存，只有某一方写内存时才复制对应页面。

### 3. 僵尸进程和孤儿进程

僵尸进程：子进程结束后，父进程没有调用 `wait()` 或 `waitpid()` 回收，导致进程表中保留退出状态。

孤儿进程：父进程先退出，子进程还在运行，子进程会被 init/systemd 接管。

排查命令：

```bash
ps -ef
ps -o pid,ppid,stat,cmd
```

`STAT` 中出现 `Z` 通常表示僵尸进程。

### 4. 多进程优缺点

优点：

- 进程之间地址空间隔离，稳定性好。
- 一个进程崩溃通常不直接破坏另一个进程。
- 适合多服务、多任务隔离。

缺点：

- 创建和切换成本比线程高。
- 进程间通信比线程间共享变量更复杂。
- 共享数据需要 IPC。

## 四、多线程

### 1. 什么是线程

线程是 CPU 调度的基本单位。同一进程内的多个线程共享进程地址空间、全局变量、堆、文件描述符等资源，但每个线程有自己的栈、寄存器上下文、线程 ID。

Linux C 常用 pthread：

```c
pthread_create();
pthread_join();
pthread_detach();
pthread_mutex_lock();
pthread_mutex_unlock();
pthread_cond_wait();
pthread_cond_signal();
```

### 2. 多线程优缺点

优点：

- 创建和切换成本比进程低。
- 线程之间共享内存，数据交换方便。
- 适合并发处理 I/O、任务队列、后台工作线程。

缺点：

- 共享数据需要同步。
- 容易出现竞态条件、死锁、内存踩踏。
- 一个线程崩溃可能导致整个进程退出。

### 3. 线程同步方式

常见方式：

- 互斥锁 `pthread_mutex_t`：保护临界区。
- 读写锁 `pthread_rwlock_t`：读多写少场景。
- 条件变量 `pthread_cond_t`：线程等待某个条件成立。
- 信号量 `sem_t`：控制资源数量。
- 原子操作：适合简单计数、状态标志。

### 4. 死锁条件

死锁通常需要同时满足四个条件：

- 互斥。
- 持有并等待。
- 不可抢占。
- 循环等待。

避免方式：

- 固定加锁顺序。
- 减小锁粒度。
- 不在持锁时调用可能阻塞的函数。
- 使用超时锁或 trylock 做保护。

## 五、内存泄漏怎么排查

### 1. 什么是内存泄漏

程序申请了资源，但不再使用后没有释放，导致进程占用持续增长。内存泄漏不只包括 `malloc` 没有 `free`，也包括文件描述符、socket、线程、定时器、句柄等资源没有释放。

### 2. 排查步骤

第一步，看现象：

```bash
top
htop
ps -o pid,rss,vsz,cmd -p <pid>
cat /proc/<pid>/status
cat /proc/<pid>/smaps
```

重点观察 RSS 是否持续增长。

第二步，定位泄漏：

```bash
valgrind --leak-check=full --show-leak-kinds=all ./program
```

或者使用 AddressSanitizer：

```bash
gcc -g -fsanitize=address -fno-omit-frame-pointer main.c -o app
ASAN_OPTIONS=detect_leaks=1 ./app
```

第三步，看代码模式：

- `malloc/calloc/realloc` 后是否所有路径都有 `free`。
- `new` 后是否对应 `delete`。
- `fopen` 后是否 `fclose`。
- `socket` 后是否 `close`。
- 错误路径和提前 `return` 是否释放资源。
- 线程退出前是否释放线程私有资源。
- 回调注册后是否有反注册。
- 链表、队列、map 中保存的指针是否释放。

### 3. 面试口述

我会先确认是不是持续增长，而不是正常缓存。然后用 `top`、`ps`、`/proc/<pid>/status` 观察 RSS，再用 valgrind 或 ASan 定位到具体申请栈。排查时重点看异常路径、循环申请、容器持有指针、线程退出、socket/fd 未关闭这些位置。

## 六、堆栈

### 1. 栈

栈由编译器和系统自动管理，常用于保存局部变量、函数参数、返回地址、寄存器现场等。

特点：

- 分配释放快。
- 生命周期随函数调用结束。
- 空间相对有限。
- 递归太深或局部大数组可能导致栈溢出。

示例：

```c
void func(void)
{
    int a = 10;
    char buf[128];
}
```

### 2. 堆

堆由程序员主动申请和释放。

常见接口：

```c
malloc();
calloc();
realloc();
free();
```

特点：

- 空间通常比栈大。
- 生命周期由程序控制。
- 容易出现内存泄漏、重复释放、越界访问、use-after-free。

### 3. 常见问题

问题：局部变量能不能返回地址？

回答：不能返回普通局部变量的地址，因为函数返回后栈帧失效，地址指向的内容不再可靠。可以返回堆内存地址、静态变量地址，或者由调用者传入缓冲区。

问题：栈溢出怎么排查？

回答：看是否有深递归、大局部数组、线程栈太小。可以用 gdb 看崩溃栈，用 `ulimit -s` 查看主线程栈大小，pthread 可以通过 `pthread_attr_setstacksize()` 设置线程栈大小。

## 七、Socket 和网络编程

### 1. TCP 服务端流程

```text
socket
setsockopt
bind
listen
accept
recv/send
close
```

典型说明：

- `socket()`：创建套接字。
- `setsockopt()`：常用 `SO_REUSEADDR`，方便端口快速复用。
- `bind()`：绑定 IP 和端口。
- `listen()`：进入监听状态。
- `accept()`：接受客户端连接。
- `recv()/send()`：收发数据。
- `close()`：关闭连接。

### 2. TCP 客户端流程

```text
socket
connect
send/recv
close
```

### 3. TCP 和 UDP 区别

TCP：

- 面向连接。
- 可靠传输。
- 保证顺序。
- 字节流。
- 有拥塞控制和流量控制。

UDP：

- 无连接。
- 不保证可靠。
- 不保证顺序。
- 数据报，有消息边界。
- 开销小，延迟低。

面试口述：

TCP 适合文件传输、登录、控制命令等可靠性要求高的场景；UDP 适合实时音视频、广播、简单查询、对延迟敏感且应用层能处理丢包的场景。

### 4. TCP 粘包和拆包

TCP 是字节流协议，没有消息边界。一次 `send()` 不一定对应一次 `recv()`。

解决方式：

- 固定长度协议。
- 包头中带长度字段。
- 使用分隔符。
- TLV 格式。

常见面试回答：

粘包不是 TCP 的 bug，而是 TCP 字节流特性。应用层必须自己设计消息边界。

### 5. recv 返回值

```text
> 0：收到的字节数
= 0：对端正常关闭连接
< 0：出错，非阻塞下可能是 EAGAIN/EWOULDBLOCK
```

### 6. 阻塞、非阻塞和 I/O 多路复用

阻塞 socket：没有数据时线程阻塞等待。

非阻塞 socket：没有数据时立即返回，通常配合 `select`、`poll`、`epoll`。

对比：

- `select`：文件描述符数量受 `FD_SETSIZE` 限制，每次需要拷贝和遍历。
- `poll`：没有固定 fd 数量限制，但仍需要遍历。
- `epoll`：适合大量连接，事件驱动，不需要每次线性扫描所有 fd。

### 7. 网络字节序

网络协议一般使用大端字节序。

常用函数：

```c
htons();
htonl();
ntohs();
ntohl();
```

面试中要强调：跨机器通信时，多字节整数不能直接按内存发送，应该统一字节序或手动序列化。

## 八、回调函数

### 1. 什么是回调函数

回调函数就是把函数指针传给另一个模块，由另一个模块在合适的时机反过来调用。

简单例子：

```c
typedef void (*event_cb)(int event, void *user_data);

void register_callback(event_cb cb, void *user_data);
```

### 2. 回调函数的作用

常见用途：

- 事件通知。
- 异步 I/O 完成通知。
- 定时器超时通知。
- 解耦模块。
- 框架调用业务逻辑。

### 3. 为什么要传 user_data

因为 C 语言函数指针本身不保存对象状态。通过 `void *user_data` 可以把调用者上下文传回回调函数，避免使用全局变量。

面试口述：

回调函数可以让底层模块不依赖上层业务逻辑。底层只保存函数指针和用户数据，事件发生时调用回调，把结果和上下文传回上层。

### 4. 回调函数注意事项

- 回调中不要做太耗时的操作，否则可能阻塞底层事件循环。
- 注意回调函数和 `user_data` 的生命周期。
- 多线程回调要注意加锁。
- 反注册回调时要避免回调正在执行。
- 不要在持锁时调用外部回调，容易造成死锁。

## 九、进程通信 IPC

### 1. 常见 IPC 方式

```text
pipe              匿名管道，常用于父子进程
FIFO              有名管道，无亲缘关系进程也能通信
message queue     消息队列，按消息传递
shared memory     共享内存，速度最快，但要自己同步
semaphore         信号量，常用于同步
signal            信号，适合简单事件通知
socket            可本机通信，也可跨机器通信
Unix domain socket 本机进程间 socket 通信，效率比 TCP socket 高
mmap              文件映射，也可用于共享内存
```

### 2. 管道

匿名管道：

- 只能用于有亲缘关系的进程。
- 半双工。
- 本质是内核缓冲区。

有名管道 FIFO：

- 通过文件系统路径标识。
- 无亲缘关系进程也可以通信。

适合简单字节流通信。

### 3. 消息队列

消息队列按消息为单位传递数据，保留消息边界。适合命令、事件、控制消息等场景。

优点：

- 有消息边界。
- 可以按类型读取。

缺点：

- 数据需要从用户态拷贝到内核，再从内核拷贝到另一个进程。
- 不适合大量大块数据传输。

### 4. 共享内存

共享内存是让多个进程把同一块物理内存映射到各自虚拟地址空间中。一个进程写入后，另一个进程可以直接读取。

常用接口：

- System V：`shmget()`、`shmat()`、`shmdt()`、`shmctl()`。
- POSIX：`shm_open()`、`mmap()`、`munmap()`、`shm_unlink()`。
- 文件映射：`mmap()`。

共享内存通常需要配合同步机制：

- 信号量。
- 互斥锁。
- futex。
- 事件通知 fd。

### 5. 为什么共享内存效率最高

共享内存效率高的核心原因是：数据不需要在两个进程之间反复经过内核拷贝。

管道、消息队列、socket 的典型数据路径：

```text
进程 A 用户态缓冲区
    -> 内核缓冲区
    -> 进程 B 用户态缓冲区
```

这里至少有用户态到内核态、内核态到用户态的数据拷贝，还伴随系统调用和调度开销。

共享内存的数据路径：

```text
进程 A 写共享内存
进程 B 直接读同一块共享内存
```

多个进程映射的是同一块物理内存，所以大块数据不需要搬来搬去。它减少了数据拷贝次数，也减少了内核参与数据转发的开销，因此在大数据量、高频通信时效率最高。

但共享内存也有代价：

- 它只解决数据共享，不负责同步。
- 需要自己处理互斥、读写顺序、数据一致性。
- 如果协议设计不好，容易出现竞态、脏读、覆盖数据。

面试口述：

共享内存快，是因为多个进程映射同一块物理内存，读写数据不需要像 pipe、消息队列、socket 那样经过内核中转拷贝。它适合大块数据和高频通信，但必须额外配合同步机制，否则会有并发一致性问题。

## 十、常见综合面试题

### 1. 多进程和多线程怎么选

如果更看重隔离性、稳定性、安全边界，选多进程。如果需要频繁共享数据、任务切换轻量、并发 I/O，选多线程。服务端程序也经常混合使用：主进程管理多个 worker 进程，每个 worker 内部再使用线程池或 epoll。

### 2. 线程崩溃会影响整个进程吗

会。多个线程共享同一个进程地址空间，一个线程非法访问内存导致段错误，通常整个进程都会退出。

### 3. 进程间能不能直接访问对方变量

不能。不同进程有独立虚拟地址空间。即使变量名和地址值看起来一样，也不是同一块可访问内存。要通信必须使用 IPC，例如共享内存、管道、消息队列或 socket。

### 4. socket 断线怎么判断

常见方式：

- `recv()` 返回 0，表示对端正常关闭。
- `recv()` 返回 -1，根据 errno 判断是否异常。
- `send()` 失败可能出现 `EPIPE`，还可能触发 `SIGPIPE`。
- 应用层心跳超时也可以判断连接失效。

工程中通常会关闭 `SIGPIPE` 或使用 `MSG_NOSIGNAL`，避免进程被信号杀掉。

### 5. 为什么 send 成功不代表对端已经收到

`send()` 成功通常只表示数据成功写入本机内核发送缓冲区，不代表对端应用层已经读取。真正的业务确认需要应用层 ACK。

### 6. 如何设计一个可靠的 TCP 应用层协议

至少包含：

- 魔数或版本号。
- 消息类型。
- payload 长度。
- 序列号。
- 校验字段。
- payload 数据。

接收端按长度字段处理粘包拆包，并对异常长度、非法类型、校验失败做防御。

### 7. 内存泄漏和内存越界有什么区别

内存泄漏是申请后没有释放，表现为内存占用持续增长。内存越界是访问了不属于自己的内存，可能导致崩溃、数据损坏或安全漏洞。valgrind 和 ASan 都可以帮助排查，但 ASan 对越界、use-after-free 通常更敏感。

### 8. select、poll、epoll 区别

`select` 受 fd 集合大小限制，每次调用需要设置 fd 集合，并且内核和用户态之间拷贝较多。`poll` 用数组管理 fd，没有固定数量限制，但仍要线性遍历。`epoll` 适合大量连接，事件就绪后返回活跃 fd，性能更好。

### 9. 回调函数和普通函数调用区别

普通函数调用是当前模块主动调用另一个函数。回调函数是当前模块把函数地址注册给框架或底层模块，之后由对方在事件发生时调用。回调的重点是反向调用和解耦。

### 10. 共享内存有什么风险

共享内存没有天然边界和同步机制。多个进程同时读写时，可能出现竞态、数据覆盖、读到半包数据等问题。所以共享内存通常要配合互斥锁、信号量、环形队列协议、读写索引和内存屏障使用。

## 十一、面试回答模板

遇到系统编程问题，可以按这个顺序回答：

```text
1. 先定义概念：它是什么。
2. 再讲使用场景：什么时候用。
3. 讲核心流程：常用 API 或执行步骤。
4. 讲风险点：会有什么坑。
5. 讲排查方法：出问题怎么定位。
6. 最后结合项目：我在项目中会怎么用。
```

例子：问共享内存为什么快。

```text
共享内存是多个进程映射同一块物理内存。它快是因为数据不需要从进程 A 拷贝到内核，再从内核拷贝到进程 B，而是一个进程写，另一个进程直接读同一块内存。所以大块数据、高频通信效率很高。但它只负责共享数据，不负责同步，实际使用时要配合信号量、互斥锁或环形队列协议。
```

