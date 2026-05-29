# MQTT 协议详解

## 1. MQTT 是什么

MQTT 是轻量级发布/订阅消息协议，常用于物联网、嵌入式设备、网关、云平台通信。

核心角色：

```text
Broker  消息服务器，例如 mosquitto、EMQX
Client  客户端，可以是设备、网关、后台服务
Topic   主题，例如 device/001/status
Publish 发布消息到某个主题
Subscribe 订阅主题
```

## 2. MQTT 和 HTTP 区别

```text
HTTP:
  请求/响应模型。
  客户端主动请求，服务端响应。
  适合接口调用、网页、REST API。

MQTT:
  发布/订阅模型。
  客户端都连接到 Broker，通过 Topic 转发消息。
  适合设备状态上报、命令下发、弱网络长连接。
```

## 3. MQTT 基本流程

```text
1. TCP connect 到 broker，默认端口 1883
2. 发送 CONNECT
3. broker 返回 CONNACK
4. PUBLISH 发布消息
5. SUBSCRIBE 订阅主题
6. PINGREQ/PINGRESP 保活
7. DISCONNECT 主动断开
```

## 4. QoS

```text
QoS 0: 最多一次。发了就不管，最快，但可能丢。
QoS 1: 至少一次。需要 PUBACK，可能重复。
QoS 2: 恰好一次。流程最复杂，开销最大。
```

## 5. 面试口述

MQTT 是物联网常用的轻量级发布订阅协议，客户端连接 Broker，通过 Topic 发布和订阅消息。它比 HTTP 更适合设备长连接、状态上报和命令下发。QoS 控制消息可靠性，KeepAlive 用于检测连接是否还活着。

## 6. 运行

先启动 broker：

```bash
sudo apt install mosquitto mosquitto-clients
mosquitto -p 1883
```

运行示例：

```bash
./build/mqtt_client_demo 127.0.0.1 1883
```

另开终端订阅：

```bash
mosquitto_sub -h 127.0.0.1 -t 'demo/device/status' -v
```
