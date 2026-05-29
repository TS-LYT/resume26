#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

// 最小 HTTP 客户端：
// 连接本机 8080 端口，发送 GET /status HTTP 请求，并打印服务器响应。
// 先运行 ../http_server 示例，再运行本示例。

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cerr << "socket failed: " << std::strerror(errno) << "\n";
        return 1;
    }

    sockaddr_in server {};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if (connect(fd, reinterpret_cast<sockaddr*>(&server), sizeof(server)) < 0) {
        std::cerr << "connect failed: " << std::strerror(errno) << "\n";
        close(fd);
        return 1;
    }

    const std::string request =
        "GET /status HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Connection: close\r\n"
        "\r\n";

    send(fd, request.c_str(), request.size(), 0);

    char buffer[1024] = {};
    ssize_t n = 0;
    while ((n = recv(fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        std::cout << buffer;
    }

    close(fd);
    return 0;
}
