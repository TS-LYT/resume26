#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

// 最小 HTTP 服务器：
// 1. socket 创建 TCP 套接字。
// 2. bind 绑定 0.0.0.0:8080。
// 3. listen 开始监听。
// 4. accept 接收客户端连接。
// 5. recv 读取 HTTP 请求文本。
// 6. send 返回 HTTP 响应文本。
//
// 这个例子重点是学习 HTTP 格式，不追求高并发和完整协议处理。

static void close_fd(int fd)
{
    if (fd >= 0) {
        close(fd);
    }
}

static std::string make_response(const std::string& body, const std::string& content_type)
{
    // HTTP 响应由三部分组成：
    // 状态行：HTTP/1.1 200 OK
    // 响应头：Content-Type、Content-Length 等
    // 空行：\r\n
    // 响应体：真正的数据
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: " + content_type + "\r\n"
           "Content-Length: " + std::to_string(body.size()) + "\r\n"
           "Connection: close\r\n"
           "\r\n" +
           body;
}

static std::string handle_request(const std::string& request)
{
    std::cout << "request:\n" << request << "\n";

    if (request.find("GET /status ") == 0) {
        return make_response("{\"online\":true,\"name\":\"http-demo\"}\n",
                             "application/json");
    }

    return make_response("hello http\n", "text/plain");
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "socket failed: " << std::strerror(errno) << "\n";
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8080);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "bind failed: " << std::strerror(errno) << "\n";
        close_fd(server_fd);
        return 1;
    }

    if (listen(server_fd, 16) < 0) {
        std::cerr << "listen failed: " << std::strerror(errno) << "\n";
        close_fd(server_fd);
        return 1;
    }

    std::cout << "HTTP server listening on http://127.0.0.1:8080\n";

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            std::cerr << "accept failed: " << std::strerror(errno) << "\n";
            continue;
        }

        char buffer[4096] = {};
        ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            std::string request(buffer, static_cast<std::size_t>(n));
            std::string response = handle_request(request);
            send(client_fd, response.c_str(), response.size(), 0);
        }

        close_fd(client_fd);
    }

    close_fd(server_fd);
    return 0;
}
