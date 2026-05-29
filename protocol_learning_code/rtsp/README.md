# RTSP 协议详解

## 1. RTSP 是什么

RTSP 全称 Real Time Streaming Protocol，常用于网络摄像机、NVR、流媒体服务器的“流控制”。

注意：RTSP 自己通常不直接承载视频数据，它主要负责控制：

```text
OPTIONS   查询服务器支持哪些方法
DESCRIBE  获取媒体描述 SDP
SETUP     建立媒体传输通道
PLAY      开始播放
PAUSE     暂停
TEARDOWN  结束会话
```

真正的视频音频数据通常通过 RTP/RTCP 传输。

## 2. RTSP 和 HTTP 的相似点

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

## 3. 典型拉流流程

```text
1. TCP connect 到摄像机 554 端口
2. OPTIONS
   查询服务端支持的方法

3. DESCRIBE
   请求 SDP，SDP 会描述视频编码、音频编码、track 地址等

4. SETUP
   为某个 track 建立 RTP 传输通道
   常见两种方式：
   - RTP over UDP：客户端告诉服务端自己的 UDP 端口
   - RTP over TCP：RTP 数据复用在 RTSP TCP 连接里

5. PLAY
   开始推 RTP 数据

6. TEARDOWN
   停止会话
```

## 4. RTP over TCP 是什么

在 NAT、防火墙或嵌入式简单测试场景中，常用 RTP over TCP。RTSP 控制消息和 RTP 数据走同一个 TCP 连接。

RTP over TCP 的数据不是普通 RTSP 文本，而是 interleaved frame：

```text
$  channel  length_hi  length_lo  RTP数据...
```

## 5. 面试口述

RTSP 是流媒体控制协议，负责建立、控制和释放媒体会话；视频数据通常由 RTP 传输。典型流程是 OPTIONS、DESCRIBE、SETUP、PLAY、TEARDOWN。DESCRIBE 返回 SDP，SETUP 决定 RTP 传输方式，PLAY 后开始收流。

## 6. 运行

```bash
./build/rtsp_client_demo 192.168.1.10 554 rtsp://192.168.1.10:554/stream
```

这个示例会发送 OPTIONS 和 DESCRIBE，打印响应。不同摄像机 URL、鉴权方式不同，实际项目要处理 Digest 鉴权、SDP 解析和 RTP 解包。
