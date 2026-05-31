#include <atomic>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

// FFmpeg 是 C 库，C++ 编译器会对函数名做 name mangling。
// extern "C" 的意思是：下面这些头文件里的函数按 C 语言链接规则处理。
// 这样链接器才能找到 libavformat/libavcodec/libavutil 里真正的 C 符号。
extern "C" {
#include <libavcodec/bsf.h>
#include <libavcodec/codec_id.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/error.h>
#include <libavutil/rational.h>
}

// Android IP Webcam 拉流小项目：
// - 使用 FFmpeg/libavformat 打开手机提供的视频 URL。
// - 打印流信息、编码信息、PTS/DTS、key frame、包大小。
// - 将 H264/H265 视频包转成 Annex-B 裸流落盘，方便用 ffplay/播放器验证。
// - 如果输入是 HTTP MJPEG，则逐帧保存为 JPEG 图片。
// - 重点练 RTSP/实时流常见坑：超时、TCP/UDP、重连、SPS/PPS、时间戳、关键帧。

// Ctrl+C 会触发 signal_handler。主循环每次读包前都会检查这个标志。
// 用 atomic 是为了让信号处理函数和主循环之间读写这个变量更稳妥。
static std::atomic_bool g_running{true};

static void signal_handler(int)
{
    g_running = false;
}

static std::string av_error(int errnum)
{
    // FFmpeg 大多数函数失败时返回负数错误码。
    // av_strerror 可以把错误码转成人能看懂的字符串，例如 "Connection timed out"。
    char buf[AV_ERROR_MAX_STRING_SIZE] = {0};
    av_strerror(errnum, buf, sizeof(buf));
    return buf;
}

// AVDictionary 是 FFmpeg 的 key-value 参数表。
// 打开输入流时可以通过它传参数，比如 RTSP 用 TCP、设置超时、设置缓冲区。
//
// 这里用一个小 RAII 包装类：对象析构时自动 av_dict_free，避免忘记释放。
class AvDictionary {
public:
    ~AvDictionary()
    {
        av_dict_free(&dict_);
    }

    AVDictionary** ptr()
    {
        return &dict_;
    }

    void set(const char* key, const char* value)
    {
        av_dict_set(&dict_, key, value, 0);
    }

private:
    AVDictionary* dict_ = nullptr;
};

// AVFormatContext 表示一个“媒体输入/输出上下文”。
// 对输入来说，它保存了容器/协议层信息，例如：
// - 输入格式：rtsp、mpjpeg、flv、mp4
// - 有几个 stream：视频、音频、字幕
// - 每个 stream 的编码参数、时间基等
//
// avformat_open_input 成功后会填充 ctx_。
// 析构时调用 avformat_close_input，会关闭网络连接并释放上下文。
class FormatContext {
public:
    ~FormatContext()
    {
        if (ctx_ != nullptr) {
            avformat_close_input(&ctx_);
        }
    }

    AVFormatContext** ptr()
    {
        return &ctx_;
    }

    AVFormatContext* get() const
    {
        return ctx_;
    }

private:
    AVFormatContext* ctx_ = nullptr;
};

// AVPacket 是“压缩后的媒体数据包”。
// 对 H264 来说，packet 里通常是一个或多个压缩视频 NALU。
// 对 MJPEG 来说，packet 里通常就是一张 JPEG 图片。
//
// 注意：AVPacket 不是解码后的 RGB/YUV 帧。
// 解码后的图像在 FFmpeg 里一般叫 AVFrame。
class Packet {
public:
    Packet()
    {
        pkt_ = av_packet_alloc();
        if (pkt_ == nullptr) {
            throw std::runtime_error("av_packet_alloc failed");
        }
    }

    ~Packet()
    {
        av_packet_free(&pkt_);
    }

    AVPacket* get() const
    {
        return pkt_;
    }

private:
    AVPacket* pkt_ = nullptr;
};

// Bitstream Filter 简称 BSF，用于转换“码流格式”。
//
// 这里最典型的是 H264：
// - RTSP/MP4 里拿到的 H264 可能是 AVCC 格式，NALU 前面是长度字段。
// - .h264 裸流常用 Annex-B 格式，NALU 前面是 00 00 01 起始码。
//
// h264_mp4toannexb 这个 filter 会把 H264 转成 Annex-B，方便 ffplay out.h264。
class BitstreamFilter {
public:
    explicit BitstreamFilter(const char* name)
    {
        const AVBitStreamFilter* filter = av_bsf_get_by_name(name);
        if (filter == nullptr) {
            throw std::runtime_error(std::string("bitstream filter not found: ") + name);
        }

        int ret = av_bsf_alloc(filter, &ctx_);
        if (ret < 0) {
            throw std::runtime_error("av_bsf_alloc failed: " + av_error(ret));
        }
    }

    ~BitstreamFilter()
    {
        av_bsf_free(&ctx_);
    }

    AVBSFContext* get() const
    {
        return ctx_;
    }

private:
    AVBSFContext* ctx_ = nullptr;
};

static void usage(const char* program)
{
    std::cout
        << "usage: " << program << " <url> <output.h264|output.h265|jpeg_dir> [max_packets]\n"
        << "examples:\n"
        << "  " << program << " rtsp://192.168.1.23:8554/live out.h264 300\n"
        << "  " << program << " http://192.168.1.23:8080/video frames 100\n"
        << "\n"
        << "IP Webcam 常见地址需要以 App 页面显示为准。浏览器能看不等于一定是 RTSP；\n"
        << "很多时候浏览器页面是 HTTP/MJPEG，这时第二个参数应写 JPEG 输出目录。\n";
}

static const char* codec_name(AVCodecID codec_id)
{
    // 把 AV_CODEC_ID_H264 这类枚举转成 "h264" 字符串，便于打印日志。
    const char* name = avcodec_get_name(codec_id);
    return name != nullptr ? name : "unknown";
}

static int find_video_stream(AVFormatContext* fmt)
{
    // 一个输入可能有多个 stream：
    // - video: H264/H265/MJPEG
    // - audio: AAC/Opus/G711
    //
    // 本示例先只处理视频，所以遍历所有 stream，找到第一个 video stream。
    for (unsigned i = 0; i < fmt->nb_streams; ++i) {
        AVStream* stream = fmt->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

static const char* choose_bsf_name(AVCodecID codec_id)
{
    // 根据编码类型选择对应的 Annex-B 转换器。
    // H265 在 FFmpeg 里也叫 HEVC。
    if (codec_id == AV_CODEC_ID_H264) {
        return "h264_mp4toannexb";
    }
    if (codec_id == AV_CODEC_ID_HEVC) {
        return "hevc_mp4toannexb";
    }
    return nullptr;
}

static bool is_jpeg_stream(AVCodecID codec_id)
{
    // IP Webcam 的 http://ip:8080/video 常见格式是 HTTP multipart MJPEG。
    // 每个 packet 基本就是一张 JPEG，直接保存为 .jpg 就能打开。
    return codec_id == AV_CODEC_ID_MJPEG;
}

static std::string jpeg_frame_path(const std::string& output_dir, int frame_index)
{
    // 生成 frames/frame_000001.jpg 这样的文件名。
    // 固定 6 位数字，后续排序和查看都更方便。
    std::ostringstream name;
    name << "frame_" << std::setw(6) << std::setfill('0') << frame_index << ".jpg";
    return (std::filesystem::path(output_dir) / name.str()).string();
}

static void print_stream_info(AVFormatContext* fmt, int video_index)
{
    AVStream* stream = fmt->streams[video_index];
    AVCodecParameters* codec = stream->codecpar;

    // codecpar 是“编码参数”，不是解码器实例。
    // 里面有 codec_id、宽高、profile、extradata 等信息。
    std::cout << "Input format: " << (fmt->iformat != nullptr ? fmt->iformat->name : "unknown") << "\n";
    std::cout << "Video stream index: " << video_index << "\n";
    std::cout << "Codec: " << codec_name(codec->codec_id) << "\n";
    std::cout << "Resolution: " << codec->width << "x" << codec->height << "\n";

    // time_base 是这个 stream 的时间单位。
    // 如果 time_base 是 1/90000，pts_delta=3000 就表示 3000/90000=0.0333 秒，约 30 FPS。
    std::cout << "Time base: " << stream->time_base.num << "/" << stream->time_base.den << "\n";

    if (stream->avg_frame_rate.num != 0 && stream->avg_frame_rate.den != 0) {
        std::cout << "Avg frame rate: " << av_q2d(stream->avg_frame_rate) << "\n";
    }
}

static void configure_input_options(AvDictionary& options, const std::string& url)
{
    // 这里按 URL 协议设置输入选项。
    //
    // RTSP 可以用 UDP 传 RTP，也可以用 TCP interleaved 传 RTP。
    // 初学和局域网调试时，TCP 更省心：
    // - 不需要额外 UDP 端口
    // - 不容易被防火墙/NAT 拦
    // - 抓包时 RTSP 控制和 RTP 数据在同一条 TCP 连接里
    //
    // 缺点是 TCP 丢包时会队头阻塞，实时性可能不如 UDP。
    if (url.rfind("rtsp://", 0) == 0) {
        options.set("rtsp_transport", "tcp");
        options.set("stimeout", "5000000"); // 旧版 FFmpeg 常用，单位微秒。
        // 不要给 RTSP demuxer 设置 timeout。部分 FFmpeg 版本会把它解释成
        // listen/server 模式超时，导致客户端拉流时报 "Unable to open RTSP for listening"。
        options.set("buffer_size", "1048576");
        options.set("max_delay", "500000");
    } else {
        // HTTP/MJPEG 这类普通 URL 常用 rw_timeout/timeout 防止网络卡死。
        options.set("rw_timeout", "5000000");
        options.set("timeout", "5000000");
    }
}

static void init_bitstream_filter(BitstreamFilter& bsf, AVStream* stream)
{
    // BSF 需要知道输入码流的编码参数，例如 codec_id、extradata。
    // extradata 里可能包含 SPS/PPS/VPS 等解码初始化信息。
    int ret = avcodec_parameters_copy(bsf.get()->par_in, stream->codecpar);
    if (ret < 0) {
        throw std::runtime_error("avcodec_parameters_copy failed: " + av_error(ret));
    }

    // time_base_in 告诉 filter 输入 packet 的时间基。
    bsf.get()->time_base_in = stream->time_base;

    // av_bsf_init 后 filter 才能开始收 packet。
    ret = av_bsf_init(bsf.get());
    if (ret < 0) {
        throw std::runtime_error("av_bsf_init failed: " + av_error(ret));
    }
}

static void write_filtered_packet(FILE* out, BitstreamFilter& bsf, AVPacket* input)
{
    // BSF 是“送进去一个 packet，可能吐出 0 个、1 个或多个 packet”的模型。
    // 所以流程是：
    // 1. av_bsf_send_packet 送输入
    // 2. 循环 av_bsf_receive_packet 取输出，直到 EAGAIN
    int ret = av_bsf_send_packet(bsf.get(), input);
    if (ret < 0) {
        throw std::runtime_error("av_bsf_send_packet failed: " + av_error(ret));
    }

    Packet filtered;
    while ((ret = av_bsf_receive_packet(bsf.get(), filtered.get())) == 0) {
        if (filtered.get()->size > 0) {
            // filtered->data 里是转换后的 Annex-B H264/H265 数据。
            // 直接写入文件，就能得到 out.h264/out.h265 裸流。
            fwrite(filtered.get()->data, 1, static_cast<size_t>(filtered.get()->size), out);
        }
        // av_packet_unref 清空 packet 引用，下一轮 receive 前必须释放旧内容。
        av_packet_unref(filtered.get());
    }

    // EAGAIN 表示当前没有更多输出，不是错误。
    // AVERROR_EOF 表示 filter 结束，也不是这里要抛出的错误。
    if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
        throw std::runtime_error("av_bsf_receive_packet failed: " + av_error(ret));
    }
}

static void write_jpeg_packet(const std::string& output_dir, int frame_index, AVPacket* packet)
{
    // MJPEG packet 通常已经是一张完整 JPEG 图片。
    // 所以不需要解码，也不需要 bitstream filter，直接把 packet->data 写成 .jpg。
    std::filesystem::create_directories(output_dir);

    const std::string path = jpeg_frame_path(output_dir, frame_index);
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("open jpeg output failed: " + path);
    }

    out.write(reinterpret_cast<const char*>(packet->data), packet->size);
    if (!out) {
        throw std::runtime_error("write jpeg output failed: " + path);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3 || argc > 4) {
        usage(argv[0]);
        return 1;
    }

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    const std::string url = argv[1];
    const std::string output_path = argv[2];
    const int max_packets = argc == 4 ? std::atoi(argv[3]) : 300;

    try {
        // FFmpeg 内部日志级别。
        // WARNING 比较适合学习：有问题能看到，正常读包时不刷太多日志。
        av_log_set_level(AV_LOG_WARNING);

        AvDictionary options;
        configure_input_options(options, url);

        FormatContext input;
        std::cout << "Opening: " << url << "\n";

        // 打开输入。
        // 对 rtsp:// URL，这一步内部会发起 RTSP 连接，并执行部分握手。
        // 对 http://.../video，这一步会建立 HTTP 连接并识别 mpjpeg。
        int ret = avformat_open_input(input.ptr(), url.c_str(), nullptr, options.ptr());
        if (ret < 0) {
            throw std::runtime_error("avformat_open_input failed: " + av_error(ret));
        }

        // 读取一部分数据，探测 stream 信息。
        // 对 RTSP 来说，这里通常会拿到 SDP 里的媒体描述，
        // 例如 video=H264、audio=Opus、payload type、time_base 等。
        ret = avformat_find_stream_info(input.get(), nullptr);
        if (ret < 0) {
            throw std::runtime_error("avformat_find_stream_info failed: " + av_error(ret));
        }

        const int video_index = find_video_stream(input.get());
        if (video_index < 0) {
            throw std::runtime_error("no video stream found");
        }

        print_stream_info(input.get(), video_index);

        AVStream* video = input.get()->streams[video_index];

        // 根据编码类型选择保存策略：
        // - MJPEG：每个 packet 直接保存成 JPEG 图片。
        // - H264/H265：通过 BSF 转 Annex-B 后保存成裸流。
        const bool jpeg_mode = is_jpeg_stream(video->codecpar->codec_id);
        const char* bsf_name = choose_bsf_name(video->codecpar->codec_id);

        if (!jpeg_mode && bsf_name == nullptr) {
            throw std::runtime_error("only H264/H265 raw output and MJPEG frame output are supported in this demo");
        }

        std::unique_ptr<BitstreamFilter> bsf;
        FILE* raw_out = nullptr;

        if (jpeg_mode) {
            std::filesystem::create_directories(output_path);
            std::cout << "Input is MJPEG. Writing JPEG frames to directory: " << output_path << "\n";
        } else {
            bsf = std::make_unique<BitstreamFilter>(bsf_name);
            init_bitstream_filter(*bsf, video);

            raw_out = std::fopen(output_path.c_str(), "wb");
            if (raw_out == nullptr) {
                throw std::runtime_error("open output failed: " + output_path);
            }
            std::cout << "Writing Annex-B stream to: " << output_path << "\n";
        }
        std::cout << "Press Ctrl+C to stop.\n";

        Packet packet;
        int video_packets = 0;
        int64_t last_pts = AV_NOPTS_VALUE;

        while (g_running && (max_packets <= 0 || video_packets < max_packets)) {
            // av_read_frame 从输入里读下一个压缩包。
            // 注意它可能读到视频包，也可能读到音频包。
            // 本示例只保存视频，所以后面会判断 stream_index。
            ret = av_read_frame(input.get(), packet.get());
            if (ret < 0) {
                std::cerr << "av_read_frame stopped: " << av_error(ret) << "\n";
                break;
            }

            if (packet.get()->stream_index == video_index) {
                // key frame 对 H264/H265 来说通常表示 IDR 或可随机访问帧。
                // 录像文件最好从 key frame 开始，否则前面可能因为缺参考帧而花屏/黑屏。
                const bool key = (packet.get()->flags & AV_PKT_FLAG_KEY) != 0;

                // PTS: Presentation Timestamp，显示时间。
                // DTS: Decoding Timestamp，解码时间。
                // 没有 B 帧时，PTS 和 DTS 通常相同；有 B 帧时可能不同。
                std::cout << "packet=" << video_packets
                          << " size=" << packet.get()->size
                          << " pts=" << packet.get()->pts
                          << " dts=" << packet.get()->dts
                          << " key=" << (key ? "yes" : "no");

                if (last_pts != AV_NOPTS_VALUE && packet.get()->pts != AV_NOPTS_VALUE) {
                    // pts_delta 可以粗略看帧间隔是否稳定。
                    // 例如 time_base=1/90000，pts_delta≈3000，说明约 30fps。
                    std::cout << " pts_delta=" << (packet.get()->pts - last_pts);
                }
                std::cout << "\n";

                if (packet.get()->pts != AV_NOPTS_VALUE) {
                    last_pts = packet.get()->pts;
                }

                if (jpeg_mode) {
                    write_jpeg_packet(output_path, video_packets, packet.get());
                } else {
                    write_filtered_packet(raw_out, *bsf, packet.get());
                }
                video_packets++;
            }

            // av_read_frame 得到的 packet 用完后要 unref。
            // 否则 packet 内部引用的缓冲区不会及时释放，长时间运行会涨内存。
            av_packet_unref(packet.get());
        }

        if (raw_out != nullptr) {
            std::fclose(raw_out);
        }

        std::cout << "Saved video packets: " << video_packets << "\n";
        if (jpeg_mode) {
            std::cout << "Try view one frame: xdg-open " << jpeg_frame_path(output_path, 0) << "\n";
        } else {
            std::cout << "Try playback: ffplay " << output_path << "\n";
        }
    } catch (const std::exception& ex) {
        std::cerr << "error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
