# CAN 协议和 Linux SocketCAN

## 1. CAN 是什么

CAN 全称 Controller Area Network，常用于汽车、工业控制、机器人、BMS、电机控制等场景。

特点：

```text
多主通信
总线仲裁
可靠性高
实时性较好
短帧传输
抗干扰能力强
```

## 2. CAN 帧基本结构

常见 Classic CAN 数据帧包括：

```text
CAN ID      标识符，也参与仲裁
DLC         数据长度，Classic CAN 为 0~8 字节
DATA        数据内容
```

标准帧 ID 是 11 bit，扩展帧 ID 是 29 bit。

## 3. 仲裁机制

CAN 总线是多主结构，多个节点可以同时尝试发送。ID 越小，优先级越高。

面试口述：

```text
CAN 使用非破坏性位仲裁。多个节点同时发时，优先级低的节点会主动退让，优先级高的帧继续发送。
```

## 4. CAN 和 TCP/UDP 区别

```text
CAN:
  总线通信，短帧，按 ID 仲裁，没有 IP 和端口概念。

TCP/UDP:
  网络通信，基于 IP 和端口，适合主机间通信。
```

## 5. Linux SocketCAN

Linux 把 CAN 设备抽象成网络接口，例如：

```text
can0
vcan0
```

常用命令：

```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0

ip -details link show vcan0
```

调试工具：

```bash
sudo apt install can-utils
candump vcan0
cansend vcan0 123#11223344
```

## 6. 运行示例

先创建虚拟 CAN：

```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
```

运行：

```bash
./build/can_socketcan_demo vcan0
```

另开终端观察：

```bash
candump vcan0
```

## 7. 面试口述

CAN 是现场总线协议，常用于车载和工业控制。它通过 CAN ID 做仲裁，ID 越小优先级越高。Linux 下通常用 SocketCAN，把 CAN 当网络设备使用，通过 `socket(PF_CAN, SOCK_RAW, CAN_RAW)` 创建套接字，再绑定 `can0` 读写 `struct can_frame`。
