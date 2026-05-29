# STL 容器学习代码

这些示例按照 `resume_stl_learning_plan.md` 的学习顺序整理。每个目录都是一个独立 CMake 小工程，可以单独编译运行。

## 学习顺序

```text
1. stage_01_vector      动态数组
2. stage_02_array       固定数组
3. stage_03_list        双向链表
4. stage_04_queue       队列
5. stage_05_stack       栈
6. stage_06_deque       双端队列
7. stage_07_projects    综合项目练习
8. stage_08_interview   面试题口述答案
9. stage_09_sqlite3     SQLite3 数据库
10. stage_10_mongoose   Mongoose 嵌入式 Web 服务器库
11. stage_11_http_https HTTP/HTTPS 基础
12. stage_12_oop        C++ 类、继承、多态、对象生命周期
```

## 统一编译方式

进入任意一个示例目录，例如：

```bash
cd stl_learning_code/stage_01_vector
cmake -S . -B build
cmake --build build
./build/demo
```

重新编译时可以删除 `build`：

```bash
rm -rf build
cmake -S . -B build
cmake --build build
./build/demo
```

## 建议学习方法

1. 先运行代码，看输出。
2. 再读 `main.cpp` 的中文注释。
3. 修改几个数字或字符串，再重新编译运行。
4. 最后打开 `stage_08_interview/interview_answers.md` 练习口述。

## 额外依赖

后续网络和数据库示例会用到系统开发库：

```bash
sudo apt install libsqlite3-dev libssl-dev
```

Mongoose 示例的 CMake 会自动下载官方 `mongoose.c` 和 `mongoose.h`，第一次编译需要能访问网络。
