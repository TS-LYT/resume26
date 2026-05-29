#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

// ONVIF 设备发现最小示例：
// 1. 创建 UDP socket。
// 2. 向 239.255.255.250:3702 发送 WS-Discovery Probe XML。
// 3. 等待设备返回 ProbeMatch。
//
// 注意：
// - 这只是发现流程，不包含用户名密码认证。
// - 获取设备信息、媒体地址等接口通常是 HTTP POST + SOAP XML。
// - 不同摄像机的网络、防火墙、ONVIF 开关都会影响发现结果。

static const char* kProbeXml =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<e:Envelope "
    "xmlns:e=\"http://www.w3.org/2003/05/soap-envelope\" "
    "xmlns:w=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\" "
    "xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\" "
    "xmlns:dn=\"http://www.onvif.org/ver10/network/wsdl\">"
    "<e:Header>"
    "<w:MessageID>uuid:12345678-1234-1234-1234-123456789abc</w:MessageID>"
    "<w:To>urn:schemas-xmlsoap-org:ws:2005:04:discovery</w:To>"
    "<w:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</w:Action>"
    "</e:Header>"
    "<e:Body>"
    "<d:Probe>"
    "<d:Types>dn:NetworkVideoTransmitter</d:Types>"
    "</d:Probe>"
    "</e:Body>"
    "</e:Envelope>";

int main(void)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    int reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in dst;
    memset(&dst, 0, sizeof(dst));
    dst.sin_family = AF_INET;
    dst.sin_port = htons(3702);
    inet_pton(AF_INET, "239.255.255.250", &dst.sin_addr);

    ssize_t sent = sendto(fd,
                          kProbeXml,
                          strlen(kProbeXml),
                          0,
                          (struct sockaddr*)&dst,
                          sizeof(dst));
    if (sent < 0) {
        perror("sendto");
        close(fd);
        return 1;
    }

    printf("ONVIF Probe sent, waiting responses for 5 seconds...\n");

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int ret = select(fd + 1, &readfds, NULL, NULL, &tv);
        if (ret < 0) {
            perror("select");
            break;
        }
        if (ret == 0) {
            printf("timeout, no more responses\n");
            break;
        }

        char buffer[8192];
        struct sockaddr_in from;
        socklen_t from_len = sizeof(from);
        ssize_t n = recvfrom(fd,
                             buffer,
                             sizeof(buffer) - 1,
                             0,
                             (struct sockaddr*)&from,
                             &from_len);
        if (n > 0) {
            buffer[n] = '\0';
            char ip[64];
            inet_ntop(AF_INET, &from.sin_addr, ip, sizeof(ip));
            printf("----- response from %s:%d -----\n%s\n",
                   ip,
                   ntohs(from.sin_port),
                   buffer);
        }
    }

    close(fd);
    return 0;
}
