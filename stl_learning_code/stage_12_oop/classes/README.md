# C++ 类：封装

## 编译运行

```bash
cmake -S . -B build
cmake --build build
./build/demo
```

## 面试口述

类把数据和操作数据的函数封装到一起，构造函数负责初始化对象，`private` 成员可以保护内部状态，外部通过 `public` 接口访问。
