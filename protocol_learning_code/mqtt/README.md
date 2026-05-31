# MQTT 协议详解和 Mongoose 实战

这份笔记按面试复习来写：先能跑代码，再把协议流程、报文结构、QoS、保活、主题匹配和常见坑讲清楚。

## 1. MQTT 是什么

MQTT 是轻量级发布/订阅消息协议，常用于物联网设备、车载终端、网关、云平台、摄像机状态上报等场景。

核心角色：

```text
Broker      消息服务器，负责接收、路由、转发消息，例如 mosquitto、EMQX
Client      客户端，可以是设备、网关、App、后台服务
Topic       主题，例如 device/001/status
Publish     发布消息到某个主题
Subscribe   订阅一个或多个主题过滤器
```

MQTT 的关键点不是“客户端直接找客户端”，而是所有客户端都连 broker。发布方只关心 topic，订阅方只关心 topic filter，两边解耦。

## 2. 本目录代码

```text
mqtt_client_demo.c       手写最小 MQTT 3.1.1 客户端，帮助理解原始报文
mqtt_mongoose_client.c   使用 Mongoose 实现 MQTT 客户端：订阅 + 定时发布
mqtt_mongoose_broker.c   使用 Mongoose 实现学习版 MQTT broker/server
```

编译：

```bash
cd protocol_learning_code
cmake -S . -B build
cmake --build build
```

运行学习版 broker：

```bash
./build/mqtt_mongoose_broker mqtt://127.0.0.1:1883
```

另开终端运行 Mongoose 客户端：

```bash
./build/mqtt_mongoose_client mqtt://127.0.0.1:1883
```

也可以用 mosquitto 客户端测试：

```bash
mosquitto_sub -h 127.0.0.1 -t 'demo/device/+/status' -v
mosquitto_pub -h 127.0.0.1 -t 'demo/device/001/cmd' -m '{"led":true}' -q 1
```

## 3. MQTT 和 HTTP 的区别

```text
HTTP:
  请求/响应模型
  客户端主动请求，服务端被动响应
  短连接或连接复用都可以
  适合 REST API、网页、文件上传下载

MQTT:
  发布/订阅模型
  客户端和 broker 保持长连接
  broker 根据 topic 转发消息
  适合设备状态上报、命令下发、弱网络、低带宽场景
```

面试可以这样说：HTTP 更像点对点接口调用，MQTT 更像消息总线。设备端不用知道谁消费消息，只要发到约定 topic。

## 4. MQTT 基本流程

典型 MQTT 3.1.1 流程：

```text
1. TCP connect 到 broker，默认端口 1883
2. 客户端发送 CONNECT
3. broker 返回 CONNACK
4. 客户端发送 SUBSCRIBE，broker 返回 SUBACK
5. 客户端发送 PUBLISH，broker 根据 topic 转发给订阅者
6. KeepAlive 到期前发送 PINGREQ，broker 返回 PINGRESP
7. 客户端主动断开时发送 DISCONNECT
```

端口常见约定：

```text
1883  MQTT over TCP，明文
8883  MQTT over TLS
8083  MQTT over WebSocket，常见但不是协议硬规定
8084  MQTT over WebSocket + TLS，常见但不是协议硬规定
```

## 5. 报文结构

MQTT 报文由三部分组成：

```text
Fixed Header      所有控制报文都有
Variable Header   部分报文有，例如 CONNECT、PUBLISH、SUBSCRIBE
Payload           部分报文有，例如 CONNECT 的 Client ID、PUBLISH 的消息体
```

固定头第一字节：

```text
高 4 bit: 控制报文类型
低 4 bit: flags
```

常见类型：

```text
1  CONNECT
2  CONNACK
3  PUBLISH
4  PUBACK
8  SUBSCRIBE
9  SUBACK
12 PINGREQ
13 PINGRESP
14 DISCONNECT
```

Remaining Length 是变长编码，每个字节只有低 7 bit 表示数值，最高 bit 表示后面还有没有字节。最大 4 字节。手写协议时很容易忘记它不是普通 `uint16_t`。

## 6. CONNECT / CONNACK

CONNECT 里重要字段：

```text
Protocol Name     MQTT
Protocol Level    4 表示 MQTT 3.1.1，5 表示 MQTT 5.0
Connect Flags     username/password/will/clean session 等标志
Keep Alive        秒级心跳间隔
Client ID         客户端唯一标识
```

CONNACK 里重要字段：

```text
Session Present   broker 是否恢复了旧会话
Return Code       0 表示连接成功，非 0 表示拒绝
```

坑：

```text
Client ID 不是随便重复的。两个客户端用同一个 Client ID 连同一个 broker，旧连接通常会被踢掉。
Clean Session=true 时断线后订阅和未完成会话状态不会保留。
KeepAlive=0 表示关闭协议层保活，不是 0 秒心跳。
用户名密码只是 MQTT 认证字段，明文 1883 上传输时仍然可能被抓包。
```

## 7. Topic 和通配符

Topic 是层级字符串，用 `/` 分隔：

```text
device/001/status
device/001/cmd
factory/a/line/2/temp
```

订阅时可以用 topic filter：

```text
+    匹配单层，例如 device/+/status 匹配 device/001/status
#    匹配多层，只能放在最后，例如 device/# 匹配 device/001/status 和 device/001/cmd/set
```

注意：

```text
发布 PUBLISH 的 topic 不能带 + 或 #，通配符只用于订阅。
topic 区分大小写。
topic 空层是合法的，例如 a//b，但项目里最好避免。
以 $ 开头的系统 topic 通常不会被普通 # 匹配，很多 broker 用 $SYS/ 暴露状态。
```

## 8. QoS 机制

QoS 0：最多一次。

```text
PUBLISH
```

特点：最快，没有确认，网络抖动时可能丢。

QoS 1：至少一次。

```text
PUBLISH -> PUBACK
```

特点：发送方没收到 PUBACK 会重发，所以接收方可能收到重复消息。业务必须能幂等，比如用消息 ID、时间戳、业务流水号去重。

QoS 2：恰好一次。

```text
PUBLISH -> PUBREC -> PUBREL -> PUBCOMP
```

特点：协议层最完整，开销最大。很多嵌入式项目为了复杂度和性能，只用 QoS0/QoS1。

面试要点：QoS 是发送方和 broker、broker 和订阅方两段分别协商的，不等于端到端数据库事务。订阅者最终收到的 QoS 不会高于发布 QoS 和订阅 QoS 的较小值。

## 9. Retain、Will、Session

Retain Message：

```text
发布时 retain=true，broker 保存该 topic 最后一条保留消息。
新订阅者订阅匹配 topic 后，会立刻收到这条消息。
常用于设备最新状态，例如 online/offline、当前温度。
```

坑：retain 不是历史消息队列，只保留每个 topic 的最后一条。发布空 payload 的 retain 消息通常用于清除保留消息。

Will Message：

```text
CONNECT 时设置遗嘱消息。
客户端异常掉线时，broker 代它发布 will。
常用于设备离线通知。
```

坑：客户端正常发送 DISCONNECT 时，broker 不会发布 will。

Session：

```text
Clean Session=true   每次新会话，不保留订阅和离线消息
Clean Session=false  broker 可保留会话状态
```

坑：是否保存离线消息还和订阅 QoS、broker 配置、持久化能力有关。

## 10. KeepAlive 和断线检测

KeepAlive 是 MQTT 层的最大静默时间。客户端在这段时间内没有发送任何控制报文，就要发 PINGREQ；broker 返回 PINGRESP。

注意：

```text
只要有 PUBLISH/SUBSCRIBE 等报文流动，就不一定需要额外 PINGREQ。
broker 通常会在 1.5 倍 KeepAlive 时间内没收到客户端报文时断开连接。
TCP keepalive 是操作系统层机制，默认可能是小时级；MQTT keepalive 是应用层机制，通常是秒级。
弱网场景必须实现重连、重新订阅、未确认消息处理。
```

## 11. Mongoose MQTT 用法

客户端核心 API：

```c
struct mg_mgr mgr;
mg_mgr_init(&mgr);

struct mg_mqtt_opts opts = {0};
opts.client_id = mg_str("demo-client");
opts.keepalive = 30;
opts.clean = true;
opts.version = 4;

mg_mqtt_connect(&mgr, "mqtt://127.0.0.1:1883", &opts, handler, NULL);
while (running) mg_mgr_poll(&mgr, 100);
mg_mgr_free(&mgr);
```

连接成功后订阅：

```c
struct mg_mqtt_opts sub = {0};
sub.topic = mg_str("demo/device/+/cmd");
sub.qos = 1;
mg_mqtt_sub(conn, &sub);
```

发布：

```c
struct mg_mqtt_opts pub = {0};
pub.topic = mg_str("demo/device/001/status");
pub.message = mg_str("{\"online\":true}");
pub.qos = 1;
mg_mqtt_pub(conn, &pub);
```

服务端/broker 核心 API：

```c
mg_mqtt_listen(&mgr, "mqtt://0.0.0.0:1883", broker_handler, NULL);
```

在 `MG_EV_MQTT_CMD` 里根据 `msg->cmd` 处理 CONNECT、SUBSCRIBE、PUBLISH、PINGREQ。

Mongoose 事件坑：

```text
MG_EV_MQTT_OPEN 的 event_data 是 int*，表示 CONNACK 状态码，不是 mg_mqtt_message*。
MG_EV_MQTT_MSG 只表示收到 PUBLISH，event_data 才是 mg_mqtt_message*。
MG_EV_MQTT_CMD 表示 MQTT 控制报文，event_data 是 mg_mqtt_message*。
mg_str 是指针+长度，不保证以 '\0' 结尾，打印要用 %.*s，保存要复制。
Mongoose 是事件循环模型，必须持续调用 mg_mgr_poll()。
回调里不要做长时间阻塞操作，否则所有连接都会被拖慢。
```

## 12. 代码实现细节

`mqtt_mongoose_client.c` 做了这些事：

```text
1. mg_mqtt_connect 连接 broker。
2. MG_EV_MQTT_OPEN 中检查 CONNACK 状态码。
3. 连接成功后订阅 demo/device/+/cmd。
4. 每 5 秒 QoS1 发布 demo/device/001/status。
5. 收到 MG_EV_MQTT_MSG 时打印 topic、payload、qos、packet id。
```

`mqtt_mongoose_broker.c` 做了这些事：

```text
1. mg_mqtt_listen 监听 mqtt://0.0.0.0:1883。
2. 收到 CONNECT 后回 CONNACK。
3. 收到 SUBSCRIBE 后保存订阅关系并回 SUBACK。
4. 收到 PUBLISH 后遍历订阅表，匹配 topic 后转发。
5. 收到 PINGREQ 后回 PINGRESP。
6. 连接关闭时清理该连接的订阅。
```

这个 broker 是学习版，不是生产版。它没有实现：

```text
认证鉴权
TLS
持久化会话
离线消息
retain 存储
QoS2 完整状态机
共享订阅
通配符和 $SYS 的全部规范边界
流量限制和最大包长度保护
```

## 13. 面试高频问题

Q：MQTT 为什么适合物联网？

A：报文头小，支持长连接和发布订阅，设备和业务端解耦，支持 QoS、KeepAlive、Will、Retain，弱网下比频繁 HTTP 轮询更省流量。

Q：QoS1 为什么会重复？

A：发送方发出 PUBLISH 后，如果 PUBACK 丢了，发送方无法判断对方是否已处理，只能重发。因此 QoS1 是至少一次，不是恰好一次。

Q：Retain 和离线消息有什么区别？

A：retain 是 broker 保存某个 topic 的最后一条状态，新订阅者立刻收到；离线消息是给持久会话客户端暂存未收到的消息。二者不是一回事。

Q：Will 消息什么时候发？

A：客户端异常断开、KeepAlive 超时、网络断开时由 broker 发布；客户端正常 DISCONNECT 时不发布。

Q：MQTT 能不能直接替代 TCP 私有协议？

A：看场景。MQTT 适合消息上报和命令下发，但如果需要严格实时、大吞吐二进制流、复杂事务或强顺序控制，可能仍要私有 TCP、HTTP/2、gRPC 或其他协议。

Q：MQTT 的安全怎么做？

A：至少使用 TLS、用户名密码或证书认证、ACL 限制 topic 权限。不要只靠 topic 命名隐藏权限。

## 14. 常见坑总结

```text
1. 忘记重连后重新订阅。
2. Client ID 重复导致连接互踢。
3. QoS1 消息重复，业务没有做幂等。
4. 把 retain 当历史消息队列。
5. 把 TCP keepalive 当 MQTT keepalive。
6. topic 大小写或通配符规则写错。
7. 发布 topic 里误用 + 或 #。
8. 回调里阻塞，导致事件循环卡住。
9. 直接把 mg_str 当 C 字符串使用。
10. 明文 1883 上传用户名密码。
11. broker 最大包长度、连接数、ACL 没配置，容易被异常客户端拖垮。
12. MQTT 5.0 和 3.1.1 字段不完全一样，库版本和协议版本要对齐。
```

## 15. 面试口述版本

MQTT 是物联网常用的轻量级发布订阅协议。客户端通过 TCP 长连接连到 broker，发布者把消息发到 topic，订阅者通过 topic filter 接收消息。它比 HTTP 轮询更适合设备状态上报和命令下发，因为连接长期保持、报文头小、通信双方解耦。可靠性由 QoS 控制：QoS0 最多一次，QoS1 至少一次可能重复，QoS2 恰好一次但开销最大。生产环境还要关注 KeepAlive、重连重订阅、Client ID 唯一性、Retain、Will、ACL、TLS 和消息幂等。
