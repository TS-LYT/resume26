# HTTPS 学习：OpenSSL 客户端

## 学什么

HTTPS = HTTP + TLS。TCP 连接建立后，先进行 TLS 握手，之后 HTTP 请求和响应都在加密通道里传输。

## 编译运行

```bash
sudo apt install libssl-dev
cmake -S . -B build
cmake --build build
./build/demo
```

默认连接 `example.com:443`，发送 `GET /` 请求并打印响应头和部分 HTML。

## 常见错误

1. 只连接 443 端口，但没有做 TLS 握手。
2. 忘记设置 SNI，部分 HTTPS 站点会握手失败或返回错误证书。
3. 不校验证书就上线，存在中间人攻击风险。
4. 嵌入式设备没有正确配置 CA 证书和系统时间。
