# 协议学习代码：RTSP、ONVIF、MQTT、CAN

这套代码用于 Linux 嵌入式应用工程师面试和项目入门。重点不是做完整商用协议栈，而是看懂协议流程、报文格式和常见代码结构。

## 目录

```text
rtsp           RTSP 拉流控制协议：OPTIONS、DESCRIBE、SETUP、PLAY
onvif          ONVIF 摄像机发现和 SOAP 请求
mqtt           MQTT 发布订阅协议：CONNECT、PUBLISH、PINGREQ、DISCONNECT
can_socketcan  Linux SocketCAN：CAN 帧发送和接收
```

## 一次性编译

```bash
cd protocol_learning_code
cmake -S . -B build
cmake --build build
```

## 运行提醒

这些示例依赖真实设备或服务：

```text
RTSP 需要摄像机或 RTSP 服务，例如 rtsp://user:pass@ip:554/stream
ONVIF 需要支持 ONVIF 的网络摄像机
MQTT 需要 broker，例如 mosquitto
CAN 需要 Linux SocketCAN 设备，例如 can0 或 vcan0
```

没有真实设备也可以先读 README 和代码注释，理解报文过程。
