#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// 这个示例手写最小 MQTT 3.1.1 客户端：
// 1. TCP connect 到 broker。
// 2. 发送 CONNECT。
// 3. 读取 CONNACK。
// 4. 发送 QoS0 PUBLISH。
// 5. 发送 PINGREQ。
// 6. 发送 DISCONNECT。
//
// 真实项目建议用成熟库：
// - Eclipse Paho
// - libmosquitto
// - mqtt-c

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

static int write_all(int fd, const uint8_t* data, size_t len)
{
    while (len > 0) {
        ssize_t n = send(fd, data, len, 0);
        if (n <= 0) {
            return -1;
        }
        data += n;
        len -= (size_t)n;
    }
    return 0;
}

static uint8_t* put_u16_string(uint8_t* p, const char* s)
{
    size_t len = strlen(s);
    *p++ = (uint8_t)((len >> 8) & 0xff);
    *p++ = (uint8_t)(len & 0xff);
    memcpy(p, s, len);
    return p + len;
}

static uint8_t* encode_remaining_length(uint8_t* p, int len)
{
    // MQTT Remaining Length 使用变长编码，每 7 bit 一组。
    do {
        uint8_t byte = (uint8_t)(len % 128);
        len /= 128;
        if (len > 0) {
            byte |= 0x80;
        }
        *p++ = byte;
    } while (len > 0);

    return p;
}

static int send_connect(int fd, const char* client_id)
{
    uint8_t packet[256];
    uint8_t variable_and_payload[200];
    uint8_t* p = variable_and_payload;

    // Variable header:
    // Protocol Name = "MQTT"
    // Protocol Level = 4 表示 MQTT 3.1.1
    // Connect Flags = 0x02 表示 Clean Session
    // Keep Alive = 60 秒
    p = put_u16_string(p, "MQTT");
    *p++ = 4;
    *p++ = 0x02;
    *p++ = 0;
    *p++ = 60;

    // Payload: Client ID
    p = put_u16_string(p, client_id);

    int remaining_len = (int)(p - variable_and_payload);

    uint8_t* out = packet;
    *out++ = 0x10; // CONNECT 固定头类型。
    out = encode_remaining_length(out, remaining_len);
    memcpy(out, variable_and_payload, remaining_len);
    out += remaining_len;

    return write_all(fd, packet, (size_t)(out - packet));
}

static int send_publish_qos0(int fd, const char* topic, const char* payload)
{
    uint8_t packet[512];
    uint8_t body[400];
    uint8_t* p = body;

    p = put_u16_string(p, topic);
    memcpy(p, payload, strlen(payload));
    p += strlen(payload);

    int remaining_len = (int)(p - body);

    uint8_t* out = packet;
    *out++ = 0x30; // PUBLISH, QoS0。
    out = encode_remaining_length(out, remaining_len);
    memcpy(out, body, remaining_len);
    out += remaining_len;

    return write_all(fd, packet, (size_t)(out - packet));
}

static void recv_connack(int fd)
{
    uint8_t buf[4];
    ssize_t n = recv(fd, buf, sizeof(buf), 0);
    if (n == 4 && buf[0] == 0x20 && buf[3] == 0x00) {
        printf("CONNACK success\n");
    } else {
        printf("CONNACK failed or unexpected response\n");
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("usage: %s <broker_host> <broker_port>\n", argv[0]);
        printf("example: %s 127.0.0.1 1883\n", argv[0]);
        return 1;
    }

    int fd = tcp_connect(argv[1], argv[2]);
    if (fd < 0) {
        printf("connect broker failed\n");
        return 1;
    }

    send_connect(fd, "linux-c-demo");
    recv_connack(fd);

    send_publish_qos0(fd, "demo/device/status", "{\"online\":true}");
    printf("PUBLISH sent\n");

    uint8_t pingreq[] = {0xC0, 0x00};
    write_all(fd, pingreq, sizeof(pingreq));
    printf("PINGREQ sent\n");

    uint8_t disconnect[] = {0xE0, 0x00};
    write_all(fd, disconnect, sizeof(disconnect));
    printf("DISCONNECT sent\n");

    close(fd);
    return 0;
}
