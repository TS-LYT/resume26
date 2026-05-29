# HTTP 学习：最小 HTTP 客户端

## 编译运行

先启动服务器：

```bash
cd ../http_server
cmake -S . -B build
cmake --build build
./build/demo
```

另开终端运行客户端：

```bash
cmake -S . -B build
cmake --build build
./build/demo
```

## 学习重点

HTTP 请求也是普通文本。最小 GET 请求包含请求行、请求头、空行：

```text
GET /status HTTP/1.1
Host: 127.0.0.1
Connection: close
```
