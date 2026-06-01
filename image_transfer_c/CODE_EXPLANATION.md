# image_transfer_c 代码作用说明

这个目录实现了一个用 C 语言编写的图片分块传输 demo。它模拟低速、半双工通信链路中的图片发送流程：发送端把图片切成固定大小的数据块逐包发送，接收端按块写入文件，并在每个周期结束后返回 ACK，ACK 中携带 bitmap 告诉发送端哪些块已经收到、哪些块需要重传。

当前 demo 使用本地二进制文件模拟通信过程，不直接操作真实网络、串口或声通模块。

## 整体设计

代码假设的传输条件如下：

- 图片最大大小为 1MB。
- 每个通信帧最大负载为 640 字节。
- 每个图片数据包携带 400 字节图片数据。
- 单程通信时间约 8 秒。
- 一个发送周期为 30 分钟。
- 接收端在周期末重复发送 3 次 ACK。
- ACK 中包含整张图片的接收 bitmap。

bitmap 的含义：

- bit 为 1：对应的图片块已经收到。
- bit 为 0：对应的图片块还没有收到。

## 文件作用

### image_transfer.h

这是整个模块的公共头文件，主要负责定义协议参数、数据包结构体、状态枚举、上下文结构体和函数声明。

重要宏定义：

- `IMAGE_MAX_SIZE`：图片最大 1MB。
- `IMAGE_CHUNK_DATA_SIZE`：每个图片分块的数据区大小，当前为 400 字节。
- `IMAGE_MAX_CHUNKS`：最大分块数量，1MB 图片按 400 字节切分时最多约 2622 块。
- `IMAGE_FULL_BITMAP_SIZE`：完整 bitmap 的最大字节数。
- `IMAGE_MAX_DATA_PER_CYCLE`：每个周期建议最多发送 210 个数据包。
- `IMG_MAGIC_CHUNK`：图片数据包魔数，用于识别 `ImageChunkPacket`。
- `IMG_MAGIC_ACK`：ACK 包魔数，用于识别 `ImageFullAckPacket`。

核心结构体：

- `ImageChunkPacket`：发送端发给接收端的图片分块数据包。
- `ImageFullAckPacket`：接收端返回给发送端的 ACK 包。
- `ImageTransferTask`：发送任务状态，记录图片大小、总块数、当前周期、下一个新块等信息。
- `ImageSendContext`：发送端上下文，保存文件句柄、已发送 bitmap、对端已接收 bitmap、重传列表等。
- `ImageRecvContext`：接收端上下文，保存输出文件句柄、接收 bitmap、已接收块数和完成状态。

### image_common.c

这个文件提供发送端和接收端都会用到的公共工具函数。

主要函数：

- `img_crc16_ccitt()`：计算 CRC16-CCITT 校验值，用于校验数据包和 ACK 是否损坏。
- `bitmap_set()`：把 bitmap 中某个分块对应的 bit 置 1。
- `bitmap_clear_bit()`：把 bitmap 中某个分块对应的 bit 清 0。
- `bitmap_is_set()`：判断某个分块是否已经被标记。
- `bitmap_clear_all()`：清空整个 bitmap。
- `image_calc_total_chunks()`：根据图片大小计算需要切成多少个分块。

### image_sender.c

这个文件实现发送端逻辑，负责读取图片、生成图片分块包、解析 ACK，并根据 ACK 生成重传列表。

主要函数：

- `sender_init()`：初始化发送端上下文，打开待发送图片，计算图片大小和总分块数。
- `sender_close()`：关闭发送端打开的图片文件。
- `sender_build_chunk()`：根据指定的 `chunk_index` 读取图片对应位置的数据，打包成 `ImageChunkPacket`，并计算 CRC。
- `sender_parse_ack()`：解析接收端返回的 ACK，校验魔数、图片序号、总块数、bitmap 长度和 CRC，并更新对端已接收 bitmap。
- `sender_build_resend_list()`：比较本地已发送 bitmap 和对端已接收 bitmap，找出已经发送但对端没收到的分块，放入重传列表。
- `sender_get_next_packet_for_cycle()`：获取当前周期要发送的下一个包。优先发送重传包，如果没有重传包，再继续发送新的图片分块。
- `sender_is_done()`：判断发送任务是否已经完成。

发送端 demo：

当编译时定义 `IMAGE_SENDER_DEMO`，`image_sender.c` 中的 `main()` 会被启用。它会读取输入图片，并把一个周期内要发送的数据包写入一个二进制文件。

示例：

```bash
./sender_demo input.jpg tx_cycle_0.bin
```

如果传入 ACK 文件，发送端会先解析 ACK，再生成重传包：

```bash
./sender_demo input.jpg tx_cycle_1.bin ack_cycle0_0.bin
```

### image_receiver.c

这个文件实现接收端逻辑，负责接收图片分块、校验 CRC、按分块序号写入输出文件，并生成 ACK。

主要函数：

- `receiver_init()`：初始化接收端上下文，创建输出图片文件，并记录图片序号、图片大小和总分块数。
- `receiver_close()`：刷新并关闭输出图片文件。
- `receiver_handle_chunk()`：处理一个图片分块包，校验魔数、图片序号、总块数、数据长度、分块序号和 CRC，然后按偏移写入文件。
- `receiver_build_ack()`：根据当前接收 bitmap 构造 ACK 包，告诉发送端已收到多少块、丢失多少块以及完整 bitmap。
- `receiver_is_done()`：判断图片是否已经完整接收。

接收端写文件时使用：

```c
offset = chunk_index * IMAGE_CHUNK_DATA_SIZE;
```

因此即使分块乱序到达，也可以写入正确位置。

接收端 demo：

当编译时定义 `IMAGE_RECEIVER_DEMO`，`image_receiver.c` 中的 `main()` 会被启用。它会读取发送端生成的二进制数据包文件，恢复输出图片，并生成 3 个 ACK 文件。

示例：

```bash
./receiver_demo tx_cycle_0.bin recv.jpg ack_cycle0
```

生成结果类似：

```text
ack_cycle0_0.bin
ack_cycle0_1.bin
ack_cycle0_2.bin
```

### Makefile

`Makefile` 用于编译发送端和接收端 demo。

主要目标：

- `make`：编译 `sender_demo` 和 `receiver_demo`。
- `make clean`：删除编译产物和 demo 生成的临时文件。

编译命令会分别定义：

- `-DIMAGE_SENDER_DEMO`：启用发送端 demo 的 `main()`。
- `-DIMAGE_RECEIVER_DEMO`：启用接收端 demo 的 `main()`。

## 传输流程

### 第 1 步：发送端切分图片并发送

发送端调用 `sender_init()` 打开图片，计算总分块数。之后反复调用 `sender_get_next_packet_for_cycle()` 获取当前周期要发送的数据包。

每个数据包包含：

- 图片序号 `image_seq`
- 当前分块序号 `chunk_index`
- 总分块数 `total_chunks`
- 图片总大小 `image_size`
- 数据长度 `data_len`
- 周期号 `cycle_index`
- 标志位 `flags`
- 图片数据 `data`
- CRC 校验值 `crc16`

### 第 2 步：接收端接收并写入图片

接收端收到 `ImageChunkPacket` 后，调用 `receiver_handle_chunk()` 处理。

处理过程包括：

1. 检查魔数和图片序号。
2. 检查分块序号和数据长度是否有效。
3. 重新计算 CRC 并和包内 CRC 对比。
4. 根据分块序号计算文件偏移。
5. 把数据写入输出图片。
6. 更新接收 bitmap 和已接收块数。

### 第 3 步：接收端生成 ACK

周期结束时，接收端调用 `receiver_build_ack()` 生成 ACK。

ACK 中包含：

- 图片序号。
- 总分块数。
- 已收到分块数。
- 丢失分块数。
- 当前周期号。
- ACK 序号。
- 图片是否完整。
- 接收 bitmap。
- CRC 校验值。

### 第 4 步：发送端解析 ACK 并重传

发送端收到 ACK 后，调用 `sender_parse_ack()` 解析并保存接收端 bitmap。

然后调用 `sender_build_resend_list()` 对比：

- 本地已经发送过的分块。
- 接收端 ACK 中标记已经收到的分块。

如果某个分块本地已经发送，但 ACK 中显示接收端没有收到，就加入重传列表。下一个周期调用 `sender_get_next_packet_for_cycle()` 时，会优先发送这些重传包。

## 代码特点

- 使用 CRC16-CCITT 对数据包和 ACK 做完整性校验。
- 使用 bitmap 记录接收状态，ACK 包较小，适合低速链路。
- 支持断续发送和按缺块重传。
- 接收端按分块序号 seek 写入文件，支持乱序接收。
- demo 使用文件模拟通信，方便在本机测试协议逻辑。

## 注意事项

1. 当前结构体使用 `#pragma pack(push, 1)` 避免编译器填充字节，但真实跨平台通信时更推荐手动序列化和反序列化。
2. 如果发送端和接收端 CPU 字节序不同，需要统一使用网络字节序。
3. 如果真实链路已有自己的帧头、帧尾、校验和重传机制，可以把 `ImageChunkPacket` 和 `ImageFullAckPacket` 当作 payload 使用。
4. 当前 demo 只模拟一轮或多轮周期数据文件，不包含真实链路收发接口。
5. `sender_demo` 和 `receiver_demo` 是编译生成的可执行文件，不是源码文件。

