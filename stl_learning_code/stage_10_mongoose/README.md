# 第 10 阶段：Mongoose 嵌入式 Web 服务器库

## 学什么

Mongoose 是 C/C++ 可用的嵌入式网络库，常用于设备 Web 配置页、REST API、WebSocket、MQTT、OTA 等场景。

官方文档的核心模型是事件驱动：初始化 `mg_mgr`，调用 `mg_http_listen` 监听端口，然后循环调用 `mg_mgr_poll()` 处理事件。

## 编译运行

第一次编译会下载 Mongoose 官方源码：

```bash
cmake -S . -B build
cmake --build build
./build/demo
```

另开一个终端测试：

```bash
curl http://127.0.0.1:8000/
curl http://127.0.0.1:8000/status
curl -X POST http://127.0.0.1:8000/echo -d "hello"
```

## 常见错误

1. 忘记持续调用 `mg_mgr_poll()`，服务器就不会处理连接。
2. 在事件回调里做耗时阻塞操作，导致其他连接也被拖慢。
3. HTTP 回调里没有正确判断 URI，所有请求都走到同一个逻辑。
4. 嵌入式工程里没有裁剪功能，导致镜像变大。

## 嵌入式注意

Mongoose 是事件驱动，不是每个连接一个线程。回调函数要短小，耗时任务应放到任务队列或工作线程。HTTPS/TLS 会增加内存和 CPU 开销，低端设备要特别评估。

## 文档来源

本示例参考 Mongoose 官方文档中 `mg_mgr_poll()`、`mg_http_listen()`、HTTP 事件回调的用法。
