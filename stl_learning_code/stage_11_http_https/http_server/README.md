# HTTP 学习：最小 HTTP 服务器

## 学什么

这个示例不用第三方 HTTP 框架，而是用 Linux socket 手写一个最小 HTTP/1.1 服务器，帮助你理解 HTTP 本质上是在 TCP 上收发文本协议。

## 编译运行

```bash
cmake -S . -B build
cmake --build build
./build/demo
```

测试：

```bash
curl http://127.0.0.1:8080/
curl http://127.0.0.1:8080/status
```

## 常见错误

1. 只发送 body，不发送 HTTP 状态行和响应头。
2. 忘记 `Content-Length`，客户端可能不知道响应何时结束。
3. 示例是单连接顺序处理，不能直接当高并发服务器。
4. 没处理半包、粘包、超长请求等工程问题。
