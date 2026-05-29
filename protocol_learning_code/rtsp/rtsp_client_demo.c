#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// 这个示例用 TCP socket 手写最小 RTSP 客户端。
// 重点看清 RTSP 报文流程：
// 1. connect 到摄像机或 RTSP 服务端。
// 2. 发送 OPTIONS。
// 3. 发送 DESCRIBE。
// 4. 打印服务端响应。
//
// 真实项目还要处理：
// - Basic/Digest 鉴权
// - SDP 解析
// - SETUP/PLAY
// - RTP over UDP 或 RTP over TCP 解包
// - H264/H265 组帧

static int tcp_connect(const char* host, const char* port)
{
    struct addrinfo hints;
    struct addrinfo* result = NULL;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port, &hints, &result) != 0) {
        return -1;
    }

    int fd = -1;
    for (struct addrinfo* p = result; p != NULL; p = p->ai_next) {
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

static int send_all(int fd, const char* data)
{
    size_t left = strlen(data);
    const char* p = data;

    while (left > 0) {
        ssize_t n = send(fd, p, left, 0);
        if (n <= 0) {
            return -1;
        }
        p += n;
        left -= (size_t)n;
    }

    return 0;
}

static void recv_and_print_once(int fd)
{
    char buffer[8192];
    memset(buffer, 0, sizeof(buffer));

    ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("----- response -----\n%s\n", buffer);
    } else if (n == 0) {
        printf("server closed connection\n");
    } else {
        perror("recv");
    }
}

static void send_rtsp_request(int fd, const char* method, const char* url, int cseq)
{
    char request[2048];

    // RTSP 请求行格式：
    // METHOD rtsp://host/path RTSP/1.0
    //
    // CSeq 是 RTSP 必须带的序号字段，服务端响应会带同样的 CSeq。
    // DESCRIBE 里 Accept: application/sdp 表示客户端希望拿到 SDP 描述。
    if (strcmp(method, "DESCRIBE") == 0) {
        snprintf(request,
                 sizeof(request),
                 "%s %s RTSP/1.0\r\n"
                 "CSeq: %d\r\n"
                 "User-Agent: rtsp-demo\r\n"
                 "Accept: application/sdp\r\n"
                 "\r\n",
                 method,
                 url,
                 cseq);
    } else {
        snprintf(request,
                 sizeof(request),
                 "%s %s RTSP/1.0\r\n"
                 "CSeq: %d\r\n"
                 "User-Agent: rtsp-demo\r\n"
                 "\r\n",
                 method,
                 url,
                 cseq);
    }

    printf("----- request -----\n%s\n", request);
    if (send_all(fd, request) != 0) {
        perror("send");
        return;
    }

    recv_and_print_once(fd);
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        printf("usage: %s <host> <port> <rtsp_url>\n", argv[0]);
        printf("example: %s 192.168.1.10 554 rtsp://192.168.1.10:554/stream\n", argv[0]);
        return 1;
    }

    const char* host = argv[1];
    const char* port = argv[2];
    const char* url = argv[3];

    int fd = tcp_connect(host, port);
    if (fd < 0) {
        printf("connect failed: %s:%s\n", host, port);
        return 1;
    }

    send_rtsp_request(fd, "OPTIONS", url, 1);
    send_rtsp_request(fd, "DESCRIBE", url, 2);

    close(fd);
    return 0;
}
