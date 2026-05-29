#include <errno.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

// Linux SocketCAN 最小示例：
// 1. socket(PF_CAN, SOCK_RAW, CAN_RAW) 创建 CAN 原始套接字。
// 2. ioctl(SIOCGIFINDEX) 根据 can0/vcan0 名称获取接口索引。
// 3. bind 绑定到指定 CAN 接口。
// 4. write 发送 struct can_frame。
// 5. read 接收 struct can_frame。
//
// 真实工程要考虑：
// - 波特率配置
// - 标准帧/扩展帧
// - 过滤器
// - 非阻塞 IO
// - bus-off 错误恢复
// - 多线程收发队列

static void print_frame(const struct can_frame* frame)
{
    printf("can_id=0x%03x dlc=%d data=", frame->can_id & CAN_EFF_MASK, frame->can_dlc);
    for (int i = 0; i < frame->can_dlc; ++i) {
        printf("%02x ", frame->data[i]);
    }
    printf("\n");
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("usage: %s <can_ifname>\n", argv[0]);
        printf("example: %s vcan0\n", argv[0]);
        return 1;
    }

    const char* ifname = argv[1];

    int fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl SIOCGIFINDEX");
        close(fd);
        return 1;
    }

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(fd);
        return 1;
    }

    struct can_frame tx;
    memset(&tx, 0, sizeof(tx));
    tx.can_id = 0x123; // 标准帧 ID。
    tx.can_dlc = 4;    // Classic CAN 数据长度最多 8 字节。
    tx.data[0] = 0x11;
    tx.data[1] = 0x22;
    tx.data[2] = 0x33;
    tx.data[3] = 0x44;

    if (write(fd, &tx, sizeof(tx)) != sizeof(tx)) {
        perror("write can frame");
        close(fd);
        return 1;
    }

    printf("sent frame: ");
    print_frame(&tx);

    printf("waiting one frame, you can run: cansend %s 456#AABBCCDD\n", ifname);

    struct can_frame rx;
    ssize_t n = read(fd, &rx, sizeof(rx));
    if (n == sizeof(rx)) {
        printf("received frame: ");
        print_frame(&rx);
    } else {
        perror("read can frame");
    }

    close(fd);
    return 0;
}
