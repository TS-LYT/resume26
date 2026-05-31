#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// 学习用 RTSP 推流示例：
// 1. OPTIONS 查询服务端能力。
// 2. ANNOUNCE 发送 SDP，告诉服务端将要推一路 H264 视频。
// 3. SETUP 建立 RTP over TCP interleaved 传输通道。
// 4. RECORD 开始推流。
// 5. 发送几帧演示用 RTP over TCP 数据。
//
// 注意：这里不做真实 H264 编码，只发送很小的演示 NALU。
// 真实推流要从摄像头/文件拿 H264/H265/AAC 帧，并按 RTP 规范分片打包。

struct rtsp_response {
    int status_code;
    char session[128];
    char raw[8192];
};

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

static int send_all_bytes(int fd, const void* data, size_t len)
{
    const uint8_t* p = (const uint8_t*)data;
    while (len > 0) {
        ssize_t n = send(fd, p, len, 0);
        if (n <= 0) {
            return -1;
        }
        p += n;
        len -= (size_t)n;
    }
    return 0;
}

static int send_all_text(int fd, const char* text)
{
    return send_all_bytes(fd, text, strlen(text));
}

static int header_value_int(const char* response, const char* name)
{
    const char* p = response;
    size_t name_len = strlen(name);

    while ((p = strstr(p, name)) != NULL) {
        if ((p == response || p[-1] == '\n') && strncasecmp(p, name, name_len) == 0 && p[name_len] == ':') {
            p += name_len + 1;
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            return atoi(p);
        }
        p += name_len;
    }

    return 0;
}

static void extract_session(const char* response, char* session, size_t session_size)
{
    const char* p = response;
    const char* name = "Session";
    size_t name_len = strlen(name);

    session[0] = '\0';
    while ((p = strstr(p, name)) != NULL) {
        if ((p == response || p[-1] == '\n') && strncasecmp(p, name, name_len) == 0 && p[name_len] == ':') {
            const char* value = p + name_len + 1;
            while (*value == ' ' || *value == '\t') {
                value++;
            }

            size_t i = 0;
            while (value[i] != '\0' && value[i] != '\r' && value[i] != '\n' &&
                   value[i] != ';' && i + 1 < session_size) {
                session[i] = value[i];
                i++;
            }
            session[i] = '\0';
            return;
        }
        p += name_len;
    }
}

static int recv_rtsp_response(int fd, struct rtsp_response* response)
{
    memset(response, 0, sizeof(*response));

    size_t used = 0;
    int content_length = -1;
    char* header_end = NULL;

    while (used + 1 < sizeof(response->raw)) {
        ssize_t n = recv(fd, response->raw + used, sizeof(response->raw) - used - 1, 0);
        if (n <= 0) {
            return -1;
        }

        used += (size_t)n;
        response->raw[used] = '\0';

        header_end = strstr(response->raw, "\r\n\r\n");
        if (header_end != NULL) {
            if (content_length < 0) {
                content_length = header_value_int(response->raw, "Content-Length");
            }

            size_t header_size = (size_t)(header_end - response->raw) + 4;
            size_t total_size = header_size + (content_length > 0 ? (size_t)content_length : 0);
            if (used >= total_size) {
                break;
            }
        }
    }

    sscanf(response->raw, "RTSP/1.0 %d", &response->status_code);
    extract_session(response->raw, response->session, sizeof(response->session));

    printf("----- response -----\n%s\n", response->raw);
    return response->status_code >= 200 && response->status_code < 300 ? 0 : -1;
}

static int send_rtsp_request(int fd,
                             const char* method,
                             const char* url,
                             int cseq,
                             const char* extra_headers,
                             const char* body,
                             struct rtsp_response* response)
{
    char request[4096];
    int body_len = body != NULL ? (int)strlen(body) : 0;

    snprintf(request,
             sizeof(request),
             "%s %s RTSP/1.0\r\n"
             "CSeq: %d\r\n"
             "User-Agent: rtsp-push-demo\r\n"
             "%s"
             "%s"
             "\r\n"
             "%s",
             method,
             url,
             cseq,
             body_len > 0 ? "Content-Type: application/sdp\r\n" : "",
             extra_headers != NULL ? extra_headers : "",
             body != NULL ? body : "");

    if (body_len > 0) {
        char content_length[64];
        snprintf(content_length, sizeof(content_length), "Content-Length: %d\r\n", body_len);

        char with_length[4096];
        snprintf(with_length,
                 sizeof(with_length),
                 "%s %s RTSP/1.0\r\n"
                 "CSeq: %d\r\n"
                 "User-Agent: rtsp-push-demo\r\n"
                 "Content-Type: application/sdp\r\n"
                 "%s"
                 "%s"
                 "\r\n"
                 "%s",
                 method,
                 url,
                 cseq,
                 content_length,
                 extra_headers != NULL ? extra_headers : "",
                 body);
        strncpy(request, with_length, sizeof(request) - 1);
        request[sizeof(request) - 1] = '\0';
    }

    printf("----- request -----\n%s\n", request);
    if (send_all_text(fd, request) != 0) {
        perror("send");
        return -1;
    }

    return recv_rtsp_response(fd, response);
}

static int send_demo_interleaved_rtp(int fd, uint16_t sequence, uint32_t timestamp)
{
    uint8_t packet[4 + 12 + 6];
    uint8_t* rtp = packet + 4;

    // RTP over TCP interleaved header: '$', channel, length_hi, length_lo。
    packet[0] = '$';
    packet[1] = 0; // RTP channel，SETUP 里 interleaved=0-1。
    packet[2] = 0;
    packet[3] = 18; // 12 字节 RTP header + 6 字节演示 payload。

    rtp[0] = 0x80; // RTP version 2。
    rtp[1] = 0x80 | 96; // marker=1, payload type=96，SDP 中映射为 H264。
    rtp[2] = (uint8_t)(sequence >> 8);
    rtp[3] = (uint8_t)(sequence & 0xff);
    rtp[4] = (uint8_t)(timestamp >> 24);
    rtp[5] = (uint8_t)(timestamp >> 16);
    rtp[6] = (uint8_t)(timestamp >> 8);
    rtp[7] = (uint8_t)(timestamp & 0xff);
    rtp[8] = 0x12;
    rtp[9] = 0x34;
    rtp[10] = 0x56;
    rtp[11] = 0x78; // SSRC。

    // 这是一个演示用 H264 IDR NALU 片段，不是完整可播放视频。
    rtp[12] = 0x65;
    rtp[13] = 0x88;
    rtp[14] = 0x84;
    rtp[15] = 0x00;
    rtp[16] = 0x2a;
    rtp[17] = 0xff;

    return send_all_bytes(fd, packet, sizeof(packet));
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        printf("usage: %s <server_host> <server_port> <rtsp_url>\n", argv[0]);
        printf("example: %s 127.0.0.1 8554 rtsp://127.0.0.1:8554/live/test\n", argv[0]);
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

    struct rtsp_response response;
    int cseq = 1;

    if (send_rtsp_request(fd, "OPTIONS", url, cseq++, NULL, NULL, &response) != 0) {
        close(fd);
        return 1;
    }

    char sdp[1024];
    snprintf(sdp,
             sizeof(sdp),
             "v=0\r\n"
             "o=- 0 0 IN IP4 127.0.0.1\r\n"
             "s=RTSP Push Demo\r\n"
             "c=IN IP4 0.0.0.0\r\n"
             "t=0 0\r\n"
             "a=control:*\r\n"
             "m=video 0 RTP/AVP 96\r\n"
             "a=rtpmap:96 H264/90000\r\n"
             "a=control:trackID=0\r\n");

    if (send_rtsp_request(fd, "ANNOUNCE", url, cseq++, NULL, sdp, &response) != 0) {
        close(fd);
        return 1;
    }

    char track_url[1024];
    snprintf(track_url, sizeof(track_url), "%s/trackID=0", url);

    const char* setup_headers = "Transport: RTP/AVP/TCP;unicast;interleaved=0-1\r\n";
    if (send_rtsp_request(fd, "SETUP", track_url, cseq++, setup_headers, NULL, &response) != 0) {
        close(fd);
        return 1;
    }

    if (response.session[0] == '\0') {
        printf("SETUP succeeded but no Session header found\n");
        close(fd);
        return 1;
    }

    char record_headers[256];
    snprintf(record_headers,
             sizeof(record_headers),
             "Session: %s\r\n"
             "Range: npt=0.000-\r\n",
             response.session);

    if (send_rtsp_request(fd, "RECORD", url, cseq++, record_headers, NULL, &response) != 0) {
        close(fd);
        return 1;
    }

    printf("Start sending demo RTP interleaved frames. They are for protocol learning, not real video.\n");
    for (int i = 0; i < 5; i++) {
        if (send_demo_interleaved_rtp(fd, (uint16_t)(1000 + i), (uint32_t)(3600 * i)) != 0) {
            perror("send rtp");
            break;
        }
        usleep(40000);
    }

    snprintf(record_headers, sizeof(record_headers), "Session: %s\r\n", response.session);
    send_rtsp_request(fd, "TEARDOWN", url, cseq++, record_headers, NULL, &response);

    close(fd);
    return 0;
}
