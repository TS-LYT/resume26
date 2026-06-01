# Image Transfer C Demo

## 设计参数

- 图片最大：1MB
- 单次通信最大负载：640B
- 单包图片数据：400B
- 单程通信时间：8s
- 周期：30分钟
- 每周期末接收端返回3次ACK
- ACK携带整张图片bitmap
- bit=1表示对应chunk已收到，bit=0表示未收到

## 文件说明

- `image_transfer.h`：协议结构体、宏定义、接口声明
- `image_common.c`：CRC16、bitmap工具函数
- `image_sender.c`：发送端逻辑
- `image_receiver.c`：接收端逻辑
- `Makefile`：编译demo

## 编译

```bash
make
```

## Demo运行方式

这个demo用本地文件模拟通信，不是真正声通链路。

### 1. 发送端生成一个周期的数据包

```bash
./sender_demo input.jpg tx_cycle_0.bin
```

### 2. 接收端读取数据包，生成输出图片和3个ACK

```bash
./receiver_demo tx_cycle_0.bin recv.jpg ack_cycle0
```

会生成：

```text
ack_cycle0_0.bin
ack_cycle0_1.bin
ack_cycle0_2.bin
```

### 3. 真实项目接入方式

你需要把下面两个地方替换成你的声通发送/接收接口：

发送数据包：

```c
send_image_chunk(ctx, chunk_index, flags);
```

实际可以改成：

```c
acoustic_send((uint8_t *)&pkt, sizeof(pkt));
```

接收ACK：

```c
sender_parse_ack(&ctx, &ack);
```

接收数据包：

```c
receiver_handle_chunk(&ctx, &pkt);
```

周期末发送ACK：

```c
receiver_build_ack(&recv_ctx, cycle_index, ack_seq, &ack);
acoustic_send((uint8_t *)&ack, sizeof(ack));
```

## 重要说明

1. 不要直接依赖结构体内存布局跨不同CPU通信，当前代码已使用 `#pragma pack(push, 1)`，但更稳的做法是手动按字节序列化。
2. 如果两端大小端不同，需要统一使用网络字节序。
3. 如果你的声通链路本身已有帧头帧尾，可以只把 `ImageChunkPacket` 和 `ImageFullAckPacket` 当作payload。
4. 发送端每周期最多建议发送210个图片包，周期末留3次ACK和保护时间。
