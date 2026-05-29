#include "mongoose.h"

#include <csignal>
#include <iostream>

// Mongoose 是 C 语言库，C++ 也可以直接使用。
// 它的核心思想是事件驱动：
// 1. mg_mgr 保存所有连接和事件循环状态。
// 2. mg_http_listen 创建 HTTP 监听端口。
// 3. mg_mgr_poll 被循环调用，负责接收连接、读写数据、触发回调。
// 4. 回调函数里根据事件类型和 URL 做处理。

static bool g_running = true;

static void signal_handler(int)
{
    // 按 Ctrl+C 时退出主循环。
    g_running = false;
}

static void http_event_handler(mg_connection* conn, int event, void* event_data)
{
    // MG_EV_HTTP_MSG 表示收到了一条完整 HTTP 请求。
    if (event != MG_EV_HTTP_MSG) {
        return;
    }

    mg_http_message* msg = static_cast<mg_http_message*>(event_data);

    // mg_match 用来匹配 URL 路径。
    // msg->uri 是 Mongoose 自己的字符串视图，不保证以 '\0' 结尾。
    if (mg_match(msg->uri, mg_str("/"), nullptr)) {
        mg_http_reply(conn,
                      200,
                      "Content-Type: text/plain\r\n",
                      "hello from mongoose\n");
    } else if (mg_match(msg->uri, mg_str("/status"), nullptr)) {
        mg_http_reply(conn,
                      200,
                      "Content-Type: application/json\r\n",
                      "{\"device\":\"linux-demo\",\"online\":true}\n");
    } else if (mg_match(msg->uri, mg_str("/echo"), nullptr)) {
        // %.*s 是打印“指定长度字符串”的常见 C 写法。
        // Mongoose 的 body 也是字符串视图，所以要使用 len。
        mg_http_reply(conn,
                      200,
                      "Content-Type: text/plain\r\n",
                      "body: %.*s\n",
                      static_cast<int>(msg->body.len),
                      msg->body.buf);
    } else {
        mg_http_reply(conn,
                      404,
                      "Content-Type: text/plain\r\n",
                      "not found\n");
    }
}

int main()
{
    std::signal(SIGINT, signal_handler);

    mg_mgr mgr;
    mg_mgr_init(&mgr);

    const char* listen_url = "http://0.0.0.0:8000";
    mg_connection* listener = mg_http_listen(&mgr, listen_url, http_event_handler, nullptr);
    if (listener == nullptr) {
        std::cerr << "listen failed: " << listen_url << "\n";
        mg_mgr_free(&mgr);
        return 1;
    }

    std::cout << "Mongoose HTTP server listening on " << listen_url << "\n";
    std::cout << "Try: curl http://127.0.0.1:8000/status\n";
    std::cout << "Press Ctrl+C to stop.\n";

    while (g_running) {
        // 1000 表示最多等待 1000ms。
        // 如果有网络事件，会调用 http_event_handler。
        mg_mgr_poll(&mgr, 1000);
    }

    mg_mgr_free(&mgr);
    return 0;
}
