# 视频编码学习笔记：H264、H265、MJPEG、码流和时间戳

这份文档配合 `rtsp_ipwebcam_recorder.cpp` 看。你现在已经从 Android IP Webcam 拉到了：

```text
rtsp://192.168.1.55:8080/h264_opus.sdp
```

程序打印出了：

```text
Codec: h264
packet=299 size=71626 pts=902641 dts=902641 key=no pts_delta=2999
```

这些日志背后就是视频编码、封装、时间戳和实时传输的核心知识。

## 1. 先分清几个概念

很多初学者会把“协议、封装、编码、像素格式”混在一起。先拆开：

```text
传输协议      RTSP、RTMP、SRT、HTTP、WebRTC
封装格式      MP4、FLV、MKV、MPEG-TS、RTP payload、裸流
视频编码      H264/AVC、H265/HEVC、MJPEG、VP8、VP9、AV1
音频编码      AAC、Opus、G711、MP3
像素格式      YUV420P、NV12、RGB24
```

可以这样理解：

```text
原始图像       摄像头采集出来的 YUV/RGB
编码           H264/H265 把原始图像压缩成码流
封装           MP4/FLV/TS/RTP 把音视频包组织起来
传输           RTSP/RTMP/SRT/HTTP 把数据送到对端
解码           把 H264/H265 还原成 YUV/RGB
显示           把图像渲染到屏幕
```

一句面试话术：

```text
H264/H265 是视频编码格式，不是传输协议；RTSP/RTMP/SRT 是传输或控制协议；MP4/FLV/TS 是封装格式。
```

## 2. 为什么要视频编码

原始视频非常大。假设一帧 1920x1080，YUV420P：

```text
一帧大小约 = 1920 * 1080 * 1.5 = 3,110,400 字节，约 3MB
30fps      = 3MB * 30 = 90MB/s
           = 720Mbps
```

这还只是 1080p，不含音频、不含传输开销。

H264/H265 的作用就是压缩：

```text
1080p 30fps H264 可能只需要 2Mbps ~ 8Mbps
1080p 30fps H265 在同等主观质量下可能比 H264 再省 30%~50%
```

压缩靠两类冗余：

```text
空间冗余   同一帧内部相邻像素相似
时间冗余   相邻帧之间画面变化不大
```

## 3. 常见视频编码

### MJPEG

MJPEG 是 Motion JPEG，本质是一张张 JPEG 图片连续播放。

特点：

```text
每帧独立压缩
实现简单
延迟低
带宽大
没有复杂帧间预测
```

你访问：

```text
http://192.168.1.55:8080/video
```

程序看到：

```text
Input format: mpjpeg
Codec: mjpeg
```

说明这个 URL 是 HTTP/MJPEG。每个 packet 基本就是一张 JPEG，所以程序可以保存：

```text
frames/frame_000000.jpg
frames/frame_000001.jpg
```

### H264 / AVC

H264 是目前最常见的视频编码之一，摄像头、RTSP、直播、MP4 都大量使用。

特点：

```text
压缩率高于 MJPEG
生态成熟
硬件编解码支持广
RTSP 摄像头常见
```

你访问：

```text
rtsp://192.168.1.55:8080/h264_opus.sdp
```

程序看到：

```text
Codec: h264
```

说明视频编码是 H264。

### H265 / HEVC

H265 是 H264 的后续标准。

特点：

```text
压缩率更高
同等画质下码率更低
编码复杂度更高
老设备兼容性不如 H264
专利和授权问题更复杂
```

安防摄像头里常见：

```text
主码流 H265
子码流 H264
```

面试里可以说：H265 适合高分辨率和存储带宽敏感场景，但兼容性和解码成本要评估。

## 4. 帧类型：I/P/B

视频编码不是每帧都完整保存。常见帧类型：

```text
I 帧   Intra frame，帧内编码，不依赖其他帧
P 帧   Predictive frame，参考之前的帧
B 帧   Bi-directional frame，参考前后帧
```

### I 帧

I 帧可以独立解码，通常也是关键帧。

特点：

```text
体积大
适合随机访问
播放器从 I 帧开始更容易正常显示
```

如果录像从 P 帧开始，前几帧可能花屏或黑屏，因为缺少参考帧。

### P 帧

P 帧只记录相对参考帧的变化。

特点：

```text
体积比 I 帧小
依赖之前的 I/P 帧
丢参考帧会影响后续画面
```

### B 帧

B 帧可以参考前后帧，压缩率更高。

特点：

```text
压缩率好
会引入编码/解码排序差异
低延迟场景常关闭 B 帧
```

有 B 帧时：

```text
PTS 和 DTS 可能不同
```

无 B 帧或低延迟流里：

```text
PTS 和 DTS 通常相同
```

你这次日志里：

```text
pts=902641 dts=902641
```

说明这个流至少在这些包上显示时间和解码时间一致，比较像低延迟摄像头流。

## 5. GOP

GOP 是 Group Of Pictures，一组视频帧。

常见结构：

```text
I P P P P P P P I P P P P P ...
```

或者有 B 帧：

```text
I B B P B B P B B I ...
```

GOP 长度表示两个关键帧之间的间隔。

例如：

```text
30fps，GOP=60
约 2 秒一个 I 帧
```

GOP 的影响：

```text
GOP 越短：关键帧更频繁，随机打开更快，但码率更高
GOP 越长：压缩更好，但首屏等待和 seek 体验变差
```

实时监控里，如果播放器打开后要等很久才出画面，常见原因就是等关键帧。

## 6. 关键帧 key frame

程序里有：

```cpp
const bool key = (packet.get()->flags & AV_PKT_FLAG_KEY) != 0;
```

日志里：

```text
key=yes
key=no
```

含义：

```text
key=yes   这个 packet 是关键帧或可随机访问帧
key=no    普通参考帧/预测帧
```

录像项目里通常要做：

```text
等第一个 key=yes 后再开始写文件
```

否则 out.h264 开头可能不是完整解码起点。

## 7. H264 码流和 NALU

H264 码流由很多 NALU 组成。

NALU 全称 Network Abstraction Layer Unit。

常见 NALU 类型：

```text
1   非 IDR 图像片，普通 P/B 帧数据
5   IDR 图像片，关键帧数据
6   SEI，补充增强信息
7   SPS，序列参数集
8   PPS，图像参数集
```

### SPS / PPS

SPS/PPS 对解码非常重要。

```text
SPS   Sequence Parameter Set，包含分辨率、profile、level 等
PPS   Picture Parameter Set，包含熵编码、slice group 等参数
```

没有 SPS/PPS，解码器可能不知道怎么解码后面的 H264 数据。

所以项目里经常要关注：

```text
SPS/PPS 是否在 SDP 里
SPS/PPS 是否在码流里周期性发送
保存裸流时是否把 SPS/PPS 写进文件
关键帧前是否有 SPS/PPS
```

## 8. Annex-B 和 AVCC

H264 常见两种存储格式。

### Annex-B

NALU 前面用起始码分隔：

```text
00 00 01
00 00 00 01
```

例如：

```text
00 00 00 01 67 ... SPS
00 00 00 01 68 ... PPS
00 00 00 01 65 ... IDR
```

`.h264` 裸流通常使用 Annex-B。

### AVCC

NALU 前面不是起始码，而是长度字段：

```text
00 00 02 5A  [NALU 数据]
```

MP4 里常见 AVCC。

### 为什么程序要 bitstream filter

程序里用了：

```text
h264_mp4toannexb
hevc_mp4toannexb
```

作用：

```text
把输入 packet 转成适合裸流保存的 Annex-B 格式
确保 ffplay out.h264 更容易识别和播放
```

这就是：

```cpp
write_filtered_packet(raw_out, *bsf, packet.get());
```

在做的事。

## 9. RTP 里的 H264

RTSP 常搭配 RTP 传 H264。

RTP 本身是传输层的包，每个 RTP 包里放 H264 数据。

H264 over RTP 常见方式：

```text
Single NALU   一个 RTP 包放一个完整小 NALU
FU-A          一个大 NALU 拆成多个 RTP 包
STAP-A        多个小 NALU 聚合成一个 RTP 包
```

### FU-A 分片

如果一个 H264 NALU 太大，超过 MTU，就要拆。

例如一个 IDR 很大：

```text
原始 NALU 80KB
网络 MTU 约 1500 字节
需要拆成很多 RTP 包
```

FU-A 有两个重要标志：

```text
S   Start，第一个分片
E   End，最后一个分片
```

接收端需要按 RTP sequence number 把它们重新组起来。

你用 FFmpeg 拉 RTSP 时，libavformat 已经帮你做了 RTP 解包和 FU-A 组帧，所以你在程序里拿到的是比较上层的 `AVPacket`。

如果你自己手写 RTSP/RTP 接收，就必须自己处理 FU-A。

## 10. PTS / DTS / time_base

程序日志：

```text
pts=902641 dts=902641 pts_delta=2999
```

含义：

```text
PTS   Presentation Timestamp，显示时间戳
DTS   Decoding Timestamp，解码时间戳
```

`time_base` 是时间戳单位。比如：

```text
Time base: 1/90000
```

如果：

```text
pts_delta=3000
```

实际时间间隔：

```text
3000 * 1/90000 = 0.0333 秒
```

也就是：

```text
约 30fps
```

你的日志里：

```text
pts_delta=2998/2999
```

说明帧间隔很接近 30fps，时间戳比较稳定。

## 11. 码率、帧率、分辨率

三个最常见参数：

```text
分辨率   1920x1080、1280x720
帧率     30fps、25fps、15fps
码率     2Mbps、4Mbps、8Mbps
```

它们的关系：

```text
分辨率越高，细节越多，需要码率越高
帧率越高，运动越流畅，需要码率越高
码率越低，压缩越狠，画质越差
```

摄像头常见主码流/子码流：

```text
主码流   高分辨率、高码率，用于录像
子码流   低分辨率、低码率，用于预览
```

## 12. CBR 和 VBR

码率控制常见两种：

```text
CBR   Constant Bitrate，恒定码率
VBR   Variable Bitrate，可变码率
```

CBR：

```text
带宽稳定
画面复杂时可能画质下降
直播和固定带宽场景常用
```

VBR：

```text
画面复杂时码率升高
画面简单时码率降低
平均画质更好
存储和点播常用
```

## 13. Profile 和 Level

H264 有 profile：

```text
Baseline   低复杂度，兼容性好，常用于低延迟
Main       更高压缩能力
High       更高画质和压缩效率，常用于高清
```

Level 表示分辨率、帧率、码率等能力限制。

比如：

```text
H264 High Profile Level 4.1
```

大概说明这个码流的复杂度和设备解码要求。

面试里不用背全部表格，知道 profile/level 用来描述编码工具集和能力上限就够。

## 14. 硬编码和软编码

软编码：

```text
CPU 编码
x264、x265
质量和参数可控
CPU 消耗高
```

硬编码：

```text
GPU/芯片编码
NVENC、VAAPI、V4L2 M2M、Android MediaCodec
速度快，功耗低
参数控制可能不如软编码细
```

摄像头、手机、NVR 通常都用硬编码。

## 15. 音频 Opus 是什么

你的 RTSP URL 叫：

```text
h264_opus.sdp
```

说明它可能包含：

```text
视频：H264
音频：Opus
```

当前程序只找第一个 video stream，所以没有保存音频。

后续如果要保存音频，需要：

```text
找到 audio stream
读取 audio packet
根据封装目标保存，例如写 MP4/MKV，而不是简单写 out.h264
处理音视频时间戳同步
```

## 16. 裸流和 MP4 的区别

`out.h264` 是裸流：

```text
只有 H264 码流
没有容器索引
没有音频
没有准确封装时间信息
适合学习和调试编码数据
```

MP4 是封装：

```text
可以同时有视频和音频
有时间戳
有索引
播放器兼容性更好
```

裸流适合学习：

```bash
ffmpeg -i out.h264 -f null -
```

但实际录像更推荐保存 MP4/MKV/TS。

## 17. 和当前程序对应起来

程序流程：

```text
1. avformat_open_input 打开 RTSP/HTTP URL
2. avformat_find_stream_info 获取 SDP/流信息
3. find_video_stream 找视频流
4. print_stream_info 打印 codec、resolution、time_base
5. av_read_frame 不断读 AVPacket
6. 判断 packet 是否属于 video stream
7. 打印 PTS/DTS/key/size
8. H264/H265 走 bitstream filter 转 Annex-B
9. MJPEG 直接保存 JPEG 图片
10. 写入文件
```

对应你跑出来的日志：

```text
Input format: rtsp
Codec: h264
Resolution: 1920x1080
Time base: 1/90000
packet=299 size=71626 pts=902641 dts=902641 key=no pts_delta=2999
```

可以解读为：

```text
这是一个 RTSP 输入
视频编码是 H264
分辨率是 1080p
时间戳单位大概率是 1/90000
这一包不是关键帧
帧间隔约 3000/90000 秒，接近 30fps
```

## 18. 常见坑

```text
1. 把 RTSP/RTMP/SRT 当成视频编码。
2. 把 MP4/FLV/TS 当成视频编码。
3. 浏览器能看就以为是 RTSP，实际可能是 HTTP/MJPEG。
4. 保存 H264 裸流时没有转 Annex-B，ffplay 播不了。
5. 录像从 P 帧开始，开头花屏或黑屏。
6. 没有 SPS/PPS，解码器无法初始化。
7. PTS/DTS 混用，导致音画不同步或播放速度异常。
8. 码率太低导致马赛克，码率太高导致网络卡顿。
9. GOP 太长导致打开慢，GOP 太短导致码率高。
10. 低延迟场景还开很多 B 帧，导致延迟增加。
11. 自己处理 RTP 时忘记 FU-A 分片重组。
12. 只保存视频裸流，却期待里面有音频和完整时间轴。
```

## 19. 面试口述版本

视频编码的作用是把原始 YUV/RGB 图像压缩成 H264/H265/MJPEG 等码流，降低带宽和存储成本。RTSP/RTMP/SRT 是传输协议，MP4/FLV/TS 是封装格式，H264/H265 才是编码格式。H264 码流由 NALU 组成，重要 NALU 包括 SPS、PPS、IDR。I 帧可以独立解码，P/B 帧依赖参考帧；GOP 决定关键帧间隔。实时流里要关注 PTS/DTS、time_base、关键帧、SPS/PPS、码率、帧率和分辨率。保存 H264 裸流时通常要转成 Annex-B 格式，否则播放器可能无法识别。
