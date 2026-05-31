# RTSP 协议详解：拉流、推流和 RTP

RTSP 全称 Real Time Streaming Protocol，常用于网络摄像机、NVR、流媒体服务器的“流控制”。面试里要讲清楚一句话：**RTSP 控制会话，RTP/RTCP 传输音视频数据**。

本目录有两个学习示例：

```text
rtsp_client_demo.c  手写 RTSP 拉流前半段：OPTIONS、DESCRIBE
rtsp_push_demo.c    手写 RTSP 推流控制流程：OPTIONS、ANNOUNCE、SETUP、RECORD、TEARDOWN
rtsp_ipwebcam_recorder.cpp  用 FFmpeg/libavformat 拉 Android IP Webcam 视频并保存 H264/H265
```

如果你正在用 Android 的 IP Webcam App 做实战项目，先看：

```text
IP_WEBCAM_PROJECT.md
```

如果你要补视频编码基础，先看：

```text
VIDEO_ENCODING_NOTES.md
```

## 1. RTSP 是什么

RTSP 报文是文本格式，长得很像 HTTP：

```text
OPTIONS rtsp://192.168.1.10:554/stream RTSP/1.0
CSeq: 1
User-Agent: demo
```

响应：

```text
RTSP/1.0 200 OK
CSeq: 1
Public: OPTIONS, DESCRIBE, SETUP, PLAY, TEARDOWN
```

常见方法：

```text
OPTIONS    查询服务端支持哪些方法
DESCRIBE   拉流时获取 SDP
ANNOUNCE   推流时发送 SDP
SETUP      建立 RTP/RTCP 传输通道
PLAY       拉流开始播放
RECORD     推流开始发送媒体
PAUSE      暂停
TEARDOWN   结束会话
```

## 2. RTSP 拉流流程

典型拉流：

```text
1. TCP connect 到摄像机或流媒体服务器，默认 554 端口
2. OPTIONS
   查询服务端支持的方法
3. DESCRIBE
   请求 SDP，拿到编码、track、payload type、控制地址
4. SETUP
   为每个 track 建立 RTP 通道，例如 video track、audio track
5. PLAY
   服务端开始发送 RTP 包
6. 客户端接收 RTP，解包、组帧、送解码器
7. TEARDOWN
   停止会话
```

拉流核心是 `DESCRIBE -> SETUP -> PLAY`。

`rtsp_client_demo.c` 目前演示了 `OPTIONS/DESCRIBE`，重点让你看清 RTSP 文本报文和 SDP 响应。真实项目还要继续做 SDP 解析、SETUP、PLAY 和 RTP 解包。

## 3. RTSP 推流流程

典型推流：

```text
1. TCP connect 到流媒体服务器
2. OPTIONS
   查询服务端能力
3. ANNOUNCE
   发送 SDP，告诉服务端自己要推什么媒体
4. SETUP
   建立 RTP 通道
5. RECORD
   通知服务端开始接收媒体
6. 推流端发送 RTP 包
7. TEARDOWN
   结束推流
```

推流核心是 `ANNOUNCE -> SETUP -> RECORD`。

`ANNOUNCE` 的 SDP 示例：

```text
v=0
o=- 0 0 IN IP4 127.0.0.1
s=RTSP Push Demo
c=IN IP4 0.0.0.0
t=0 0
a=control:*
m=video 0 RTP/AVP 96
a=rtpmap:96 H264/90000
a=control:trackID=0
```

这里的重点：

```text
m=video 0 RTP/AVP 96      表示一路视频，payload type 是动态类型 96
a=rtpmap:96 H264/90000    说明 96 对应 H264，时钟频率 90000
a=control:trackID=0       SETUP 时通常访问 rtsp://.../trackID=0
```

## 4. RTP over UDP 和 RTP over TCP

RTSP 只控制，真正媒体数据通过 RTP。

RTP over UDP：

```text
SETUP ... RTSP/1.0
Transport: RTP/AVP;unicast;client_port=5000-5001
```

特点：

```text
RTP 和 RTCP 走单独 UDP 端口
延迟低
容易受 NAT、防火墙、丢包影响
```

RTP over TCP：

```text
SETUP ... RTSP/1.0
Transport: RTP/AVP/TCP;unicast;interleaved=0-1
```

特点：

```text
RTSP 控制消息和 RTP/RTCP 数据复用同一个 TCP 连接
穿透 NAT/防火墙更容易
TCP 丢包会队头阻塞，实时性可能变差
```

RTP over TCP 的 interleaved frame 格式：

```text
'$'  channel  length_hi  length_lo  RTP/RTCP payload...
```

例如 `interleaved=0-1` 通常表示：

```text
channel 0  RTP
channel 1  RTCP
```

## 5. RTP 包结构

RTP header 常见 12 字节：

```text
V/P/X/CC
M/PT
Sequence Number
Timestamp
SSRC
```

关键字段：

```text
Version           RTP 版本，通常是 2
Payload Type      负载类型，例如 96 动态映射到 H264
Marker            一帧结束标志，视频里常用于标记一帧最后一个 RTP 包
Sequence Number   RTP 包序号，用于检测丢包和乱序
Timestamp         采样时间戳，H264 常用 90000Hz 时钟
SSRC              同步源标识
```

## 6. H264 over RTP

RTSP 里最常见的是 H264/H265 over RTP。H264 RTP 打包常见三类：

```text
Single NAL Unit Packet   一个 RTP 包放一个完整小 NALU
FU-A                     大 NALU 分片，多包组成一个 NALU
STAP-A                   多个小 NALU 聚合到一个 RTP 包
```

面试最常问 FU-A：

```text
原始 NALU 太大，超过 MTU，需要拆成多个 RTP 包。
第一个分片 S=1，最后一个分片 E=1。
接收端按 sequence number 组回原始 NALU。
```

H264 常见 NALU：

```text
SPS  序列参数集，解码器初始化需要
PPS  图像参数集，解码器初始化需要
IDR  关键帧
P/B  普通预测帧
```

坑：推流开始时通常要让客户端尽快拿到 SPS/PPS/IDR，否则播放器可能黑屏等关键帧。

## 7. SDP 重点字段

SDP 是 RTSP 里的媒体说明书。常见字段：

```text
v=0                         SDP 版本
o=- ...                     origin
s=...                       session name
c=IN IP4 0.0.0.0            connection info
t=0 0                       time
m=video 0 RTP/AVP 96        media line
a=rtpmap:96 H264/90000      payload type 映射
a=fmtp:96 ...               编码参数，例如 sprop-parameter-sets
a=control:trackID=0         track 控制 URL
```

拉流时，客户端从 SDP 里解析 track URL、编码类型、payload type、SPS/PPS 等信息。

推流时，推流端通过 ANNOUNCE 把 SDP 发给服务器，告诉服务器即将收到什么格式的 RTP。

## 8. 鉴权

RTSP 摄像机常见 Basic 和 Digest 鉴权。

Basic：

```text
Authorization: Basic base64(username:password)
```

Digest：

```text
服务端先返回 401，带 WWW-Authenticate: Digest realm=..., nonce=...
客户端按 username、password、realm、nonce、method、uri 计算 response
再次发送请求并带 Authorization: Digest ...
```

坑：

```text
RTSP 鉴权通常每个请求都要带 Authorization。
CSeq 每次请求递增，401 重试时也要注意序号。
很多摄像机的 Digest 实现有兼容性差异，工程里要留日志。
```

## 9. 运行示例

编译：

```bash
cd protocol_learning_code
cmake -S . -B build
cmake --build build
```

拉流前半段：

```bash
./build/rtsp_client_demo 192.168.1.10 554 rtsp://192.168.1.10:554/stream
```

推流控制流程：

```bash
./build/rtsp_push_demo 127.0.0.1 8554 rtsp://127.0.0.1:8554/live/test
```

可以用支持 RTSP 推流的服务端测试，例如 MediaMTX、ZLMediaKit、EasyDarwin 等。`rtsp_push_demo.c` 发送的是演示 RTP 包，不是真实可播放视频；它的目标是帮你看清 `ANNOUNCE/SETUP/RECORD/RTP over TCP` 这条链路。

## 10. 推流项目里真正要做什么

真实 RTSP 推流通常分成几层：

```text
采集层      摄像头、文件、编码器输出 H264/H265/AAC
封包层      H264/H265/AAC 按 RTP 规范打包，处理分片、时间戳、序号
控制层      RTSP OPTIONS/ANNOUNCE/SETUP/RECORD/TEARDOWN
传输层      UDP 或 TCP interleaved 发送 RTP/RTCP
状态层      重连、保活、码率统计、丢包统计、错误恢复
```

推 H264 的关键实现点：

```text
1. 解析 Annex-B 起始码 00 00 01 / 00 00 00 01。
2. 拆出 NALU。
3. 小 NALU 直接 Single NALU RTP 打包。
4. 大 NALU 按 MTU 做 FU-A 分片。
5. 每个 RTP 包递增 sequence number。
6. 同一帧使用同一个 timestamp。
7. 一帧最后一个 RTP 包 marker=1。
8. 推流开始或关键帧前保证 SPS/PPS 可达。
```

## 11. 常见坑

```text
1. 只实现 RTSP 控制，没有正确打 RTP 包，播放器仍然播不了。
2. SETUP 的 track URL 拼错，尤其是 SDP 里 control 是相对路径时。
3. RTP over TCP 没处理 '$' interleaved frame，误当成 RTSP 文本。
4. Content-Length 计算错误，ANNOUNCE/DESCRIBE body 读不完整。
5. CSeq 不递增或响应 CSeq 对不上。
6. Session 头没保存，PLAY/RECORD/TEARDOWN 缺 Session。
7. H264 大帧不做 FU-A 分片，超过 MTU 后丢包严重。
8. timestamp 乱跳，播放器卡顿或倍速异常。
9. marker bit 使用错误，接收端组帧困难。
10. SPS/PPS 没发或只在 SDP 里有但实际流里没有，播放器黑屏。
11. UDP 端口被 NAT/防火墙拦截，改用 RTP over TCP 更容易排查。
12. 401 Digest 鉴权处理不完整。
```

## 12. 面试口述版本

RTSP 是流媒体控制协议，负责建立、控制和释放媒体会话，真正音视频一般走 RTP/RTCP。拉流典型流程是 `OPTIONS -> DESCRIBE -> SETUP -> PLAY -> TEARDOWN`，其中 DESCRIBE 返回 SDP，客户端根据 SDP 建立 track 的 RTP 通道。推流典型流程是 `OPTIONS -> ANNOUNCE -> SETUP -> RECORD -> TEARDOWN`，ANNOUNCE 把本端媒体 SDP 发给服务器，RECORD 后开始发 RTP。RTP 可以走 UDP，也可以通过 interleaved 方式复用在 RTSP TCP 连接里。项目里最容易出问题的是 SDP 解析、Session/CSeq 管理、Digest 鉴权、RTP 分片组帧、时间戳和关键帧参数集。
