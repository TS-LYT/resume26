# 第 11 阶段：HTTP / HTTPS 学习代码

## 目录说明

```text
http_server   用 Linux socket 手写最小 HTTP 服务器
http_client   用 Linux socket 手写最小 HTTP 客户端
https_client  用 OpenSSL 手写最小 HTTPS 客户端
```

## 学习顺序

1. 先看 `http_server`，理解 HTTP 响应格式。
2. 再看 `http_client`，理解 HTTP 请求格式。
3. 最后看 `https_client`，理解 HTTPS 多了 TLS 握手和证书校验。

## 面试口述

HTTP 是应用层明文协议，通常跑在 TCP 上。HTTPS 可以理解为 HTTP 加 TLS，TLS 负责加密、完整性保护和身份认证。嵌入式设备使用 HTTPS 时，要注意 CA 证书、系统时间、内存占用和握手耗时。
