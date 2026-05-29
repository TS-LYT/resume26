# 第 9 阶段：SQLite3 数据库练习

## 学什么

SQLite3 是一个轻量级嵌入式数据库，数据通常保存在单个 `.db` 文件里。Linux 嵌入式应用里，它适合保存配置、历史记录、离线缓存、采样数据等。

## 和 C 语言经验的关系

SQLite3 官方接口是 C API，C++ 可以直接调用。核心流程是：

```text
sqlite3_open -> sqlite3_exec 建表/写入 -> sqlite3_prepare_v2 查询 -> sqlite3_step 读行 -> sqlite3_finalize -> sqlite3_close
```

## 编译运行

```bash
sudo apt install libsqlite3-dev
cmake -S . -B build
cmake --build build
./build/demo
```

运行后会生成 `sensor.db`。

## 常见错误

1. 忘记检查 SQLite 返回值。
2. `sqlite3_prepare_v2` 后忘记 `sqlite3_finalize`。
3. 字符串拼 SQL，容易出错；实际工程应优先使用绑定参数。
4. 多线程同时访问同一个数据库时，没有设计好锁和事务。

## 嵌入式注意

频繁写数据库会带来 Flash/SD 卡磨损和 IO 阻塞。工程中要控制写入频率，必要时批量写入、使用事务，并考虑断电保护。
