# Android IP Webcam 拉流采集项目

目标：用 Android 手机上的 IP Webcam 当视频源，写一个 C++ 程序拉流、分析视频包、落盘 H264/H265 裸流，并在过程中踩到 RTSP/实时流开发里最常见的点。

对应代码：

```text
rtsp_ipwebcam_recorder.cpp
```

## 1. 先确认手机视频源

手机和电脑必须在同一个局域网。IP Webcam 打开后，浏览器一般访问：

```text
http://手机IP:8080/
```

浏览器看到画面，说明 HTTP 页面通了。但要注意：**浏览器能看，不代表你拿到的是 RTSP**。很多 IP Webcam App 默认网页画面是 HTTP MJPEG，例如：

```text
http://手机IP:8080/video
```

RTSP 地址通常需要在 App 设置或网页菜单里找，常见形态可能是：

```text
rtsp://手机IP:8554/live
rtsp://手机IP:8080/h264_ulaw.sdp
rtsp://手机IP:8080/video/h264
```

具体以 App 页面显示为准。

## 2. 安装依赖

Ubuntu/Debian：

```bash
sudo apt update
sudo apt install pkg-config libavformat-dev libavcodec-dev libavutil-dev ffmpeg
```

`ffmpeg` 命令用于验证，`libavformat-dev/libavcodec-dev/libavutil-dev` 用于编译 C++ 程序。

## 3. 编译

```bash
cd protocol_learning_code
cmake -S . -B build
cmake --build build
```

如果没有安装 FFmpeg 开发库，CMake 会跳过 `rtsp_ipwebcam_recorder`，并提示安装命令。

## 4. 先用工具验证 URL

RTSP：

```bash
ffplay -rtsp_transport tcp rtsp://手机IP:8554/live
```

HTTP/MJPEG：

```bash
ffplay http://手机IP:8080/video
```

如果 `ffplay` 都打不开，先不要怀疑代码，优先排查：

```text
手机和电脑是否同一 Wi-Fi
手机 App 是否还在前台运行或允许后台运行
防火墙是否拦截
URL 路径是否正确
App 是否需要用户名密码
视频编码是否启用了 H264/H265
```

## 5. 运行 C++ 采集器

RTSP 示例：

```bash
./build/rtsp_ipwebcam_recorder rtsp://手机IP:8554/live out.h264 300
```

HTTP 示例：

```bash
./build/rtsp_ipwebcam_recorder http://手机IP:8080/video frames 100
```

参数含义：

```text
第 1 个参数：输入 URL
第 2 个参数：H264/H265 时是输出裸流文件；MJPEG 时是 JPEG 帧输出目录
第 3 个参数：最多保存多少个视频包，0 表示一直保存到 Ctrl+C
```

播放落盘文件：

```bash
ffplay out.h264
```

如果是 H265：

```bash
./build/rtsp_ipwebcam_recorder rtsp://手机IP:8554/live out.h265 300
ffplay out.h265
```

如果 IP Webcam 显示：

```text
Input format: mpjpeg
Codec: mjpeg
```

说明这个 URL 是 HTTP/MJPEG，不是 RTSP/H264。程序会把每帧保存成 JPEG：

```bash
./build/rtsp_ipwebcam_recorder http://手机IP:8080/video frames 100
xdg-open frames/frame_000000.jpg
```

## 6. 这个项目练什么

程序做了这些事：

```text
1. avformat_open_input 打开实时流 URL。
2. 给 RTSP 设置 rtsp_transport=tcp，优先使用 RTP over TCP。
3. 设置 timeout/stimeout/rw_timeout，避免网络异常时一直卡死。
4. avformat_find_stream_info 获取编码、分辨率、time_base。
5. 找到 video stream。
6. 打印每个视频包的 size、pts、dts、key frame。
7. H264/H265 使用 h264_mp4toannexb 或 hevc_mp4toannexb bitstream filter。
8. H264/H265 转成 Annex-B 裸流保存；MJPEG 逐帧保存为 JPEG 图片。
```

你能从日志里观察：

```text
是否有关键帧
每帧包大小
PTS 是否递增
PTS delta 是否稳定
断线时 av_read_frame 返回什么错误
手机切后台或锁屏后流会不会断
Wi-Fi 抖动时是否卡住
```

## 7. 必踩 RTSP 注意点

### URL 不等于页面地址

IP Webcam 的首页是 HTTP 控制页面，视频源可能有多个：

```text
HTTP MJPEG
RTSP H264
JPEG snapshot
音频流
```

要确认自己打开的是 RTSP 还是 HTTP。RTSP 才会走 `OPTIONS/DESCRIBE/SETUP/PLAY`。

### RTSP TCP 和 UDP

本项目默认：

```text
rtsp_transport=tcp
```

这样 RTP 通过 RTSP TCP 连接 interleaved 传输，穿透局域网、防火墙更省心。缺点是 TCP 丢包会队头阻塞，实时性可能变差。

如果要练 UDP，可以把代码里的 `rtsp_transport` 改成：

```text
udp
```

然后观察丢包、端口、防火墙问题。

### 超时必须设置

实时流最怕网络断了但程序卡死。本项目设置了：

```text
stimeout
timeout
rw_timeout
```

不同 FFmpeg 版本支持的 option 可能不完全一致，所以代码里做了多项设置。工程里通常还要做重连和状态机。

### SPS/PPS 和 Annex-B

播放器解码 H264 需要 SPS/PPS。RTSP/MP4/裸流里的 H264 格式可能不一样：

```text
AVCC 格式       常见于 MP4，NALU 前面是长度
Annex-B 格式    常见于 .h264 裸流，NALU 前面是 00 00 01 起始码
```

本项目使用 FFmpeg bitstream filter：

```text
h264_mp4toannexb
hevc_mp4toannexb
```

作用是把视频包转成更适合保存和播放的 Annex-B 裸流。

### PTS/DTS 和实时性

日志里会打印：

```text
pts
dts
pts_delta
key
```

注意：

```text
PTS 用于显示时间
DTS 用于解码顺序
有 B 帧时 PTS 和 DTS 可能不同
实时流里有些设备时间戳可能不稳定
时间戳乱跳会导致播放器卡顿、快进、延迟异常
```

### 关键帧

如果开始拉流时刚好没拿到关键帧，播放器可能短暂黑屏。正常项目里通常要：

```text
等待 key frame 后再开始录像
推流端定期发送 IDR
保证 SPS/PPS 可达
```

### 断线重连

手机锁屏、App 切后台、Wi-Fi 漫游都会导致流断。生产代码应该有：

```text
读取失败检测
退避重连
重新打开输入
重新找流
重新初始化 bitstream filter
文件切片或断点处理
```

当前示例先把错误打印出来，方便你观察真实现象。

## 8. 可以扩展成具体项目

建议项目名：`ipcam_recorder`

功能路线：

```text
第 1 版：拉流并保存 out.h264
第 2 版：只从关键帧开始保存
第 3 版：断线自动重连
第 4 版：每 60 秒切一个文件
第 5 版：统计码率、帧率、丢包/卡顿
第 6 版：加一个 HTTP 接口查看当前状态
第 7 版：对接 SQLite 保存录像索引
```

这样你就能把 C++、RTSP、HTTPS/HTTP、数据库都串起来。

## 9. 面试口述版本

我用 Android IP Webcam 搭了一个局域网视频源，C++ 侧用 FFmpeg 的 libavformat 打开 RTSP/HTTP 流。RTSP 默认使用 TCP 传输，避免 UDP 端口和 NAT 问题；打开流时设置超时，避免网络异常卡死。程序会读取视频 stream，打印编码、分辨率、time_base、PTS/DTS、关键帧和包大小，并用 bitstream filter 把 H264/H265 转成 Annex-B 裸流落盘。这个过程中重点处理 URL 类型判断、RTSP TCP/UDP 选择、超时、关键帧、SPS/PPS、时间戳和断线重连这些实时流常见问题。
