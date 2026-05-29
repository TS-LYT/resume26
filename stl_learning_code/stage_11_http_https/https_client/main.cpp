#include <cstring>
#include <iostream>
#include <netdb.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

// HTTPS 客户端学习示例：
// 1. 先用 TCP 连接 example.com:443。
// 2. 创建 OpenSSL SSL_CTX 和 SSL 对象。
// 3. 设置 SNI，让服务器知道我们要访问哪个域名。
// 4. SSL_connect 完成 TLS 握手。
// 5. 通过 SSL_write/SSL_read 发送和读取 HTTP 文本。

static int tcp_connect(const char* host, const char* port)
{
    addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* result = nullptr;
    if (getaddrinfo(host, port, &hints, &result) != 0) {
        return -1;
    }

    int fd = -1;
    for (addrinfo* p = result; p != nullptr; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0) {
            continue;
        }

        if (connect(fd, p->ai_addr, p->ai_addrlen) == 0) {
            break;
        }

        close(fd);
        fd = -1;
    }

    freeaddrinfo(result);
    return fd;
}

int main()
{
    const char* host = "example.com";
    const char* port = "443";

    int fd = tcp_connect(host, port);
    if (fd < 0) {
        std::cerr << "tcp connect failed\n";
        return 1;
    }

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == nullptr) {
        std::cerr << "SSL_CTX_new failed\n";
        close(fd);
        return 1;
    }

    // 加载系统默认 CA，用于校验证书。
    SSL_CTX_set_default_verify_paths(ctx);

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, fd);
    SSL_set_tlsext_host_name(ssl, host); // SNI
    SSL_set1_host(ssl, host);            // 证书主机名校验
    SSL_set_verify(ssl, SSL_VERIFY_PEER, nullptr);

    if (SSL_connect(ssl) != 1) {
        std::cerr << "TLS handshake failed\n";
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(fd);
        return 1;
    }

    const std::string request =
        "GET / HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Connection: close\r\n"
        "\r\n";

    SSL_write(ssl, request.c_str(), static_cast<int>(request.size()));

    char buffer[2048] = {};
    int n = 0;
    while ((n = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        std::cout << buffer;
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(fd);
    return 0;
}
