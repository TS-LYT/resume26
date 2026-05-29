# Socket 和网络编程基础

## TCP 服务端流程

```text
socket
setsockopt(SO_REUSEADDR)
bind
listen
accept
recv/send
close
```

## TCP 客户端流程

```text
socket
connect
send/recv
close
```

## 面试常问

### 1. TCP 和 UDP 区别？

TCP 面向连接、可靠、有序、字节流；UDP 无连接、不保证可靠和顺序，但开销小，适合实时音视频、广播、简单请求响应等场景。

### 2. recv 返回 0 表示什么？

对端正常关闭连接。

### 3. 粘包是什么？

TCP 是字节流，没有消息边界。一次 send 不一定对应一次 recv。应用层需要自己设计协议边界，例如固定长度、长度字段、分隔符。

### 4. 阻塞和非阻塞区别？

阻塞 socket 没有数据时会等待；非阻塞 socket 会立刻返回错误码，通常配合 `select/poll/epoll` 使用。

## 运行

终端 1：

```bash
./build/tcp_server
```

终端 2：

```bash
./build/tcp_client
```
