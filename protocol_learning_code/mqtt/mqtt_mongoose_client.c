#include "mongoose.h"

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Mongoose MQTT 客户端示例：
// 1. 连接 broker。
// 2. 连接成功后订阅 demo/device/+/cmd。
// 3. 定时发布 demo/device/001/status。
// 4. 收到订阅消息时打印 topic 和 payload。

static volatile sig_atomic_t s_running = 1;
static struct mg_connection* s_mqtt_conn = NULL;
static bool s_mqtt_ready = false;

static const char* s_sub_topic = "demo/device/+/cmd";
static const char* s_pub_topic = "demo/device/001/status";

static void signal_handler(int signo)
{
    (void)signo;
    s_running = 0;
}

static void mqtt_event_handler(struct mg_connection* conn, int event, void* event_data)
{
    if (event == MG_EV_MQTT_OPEN) {
        int ack = *(int*)event_data;
        if (ack != 0) {
            printf("MQTT CONNACK failed, return code=%d\n", ack);
            conn->is_closing = 1;
            return;
        }

        struct mg_mqtt_opts sub_opts;
        memset(&sub_opts, 0, sizeof(sub_opts));
        sub_opts.topic = mg_str(s_sub_topic);
        sub_opts.qos = 1;
        mg_mqtt_sub(conn, &sub_opts);

        s_mqtt_ready = true;
        printf("MQTT connected, subscribed: %s\n", s_sub_topic);
    } else if (event == MG_EV_MQTT_MSG) {
        struct mg_mqtt_message* msg = (struct mg_mqtt_message*)event_data;
        printf("RECV topic=%.*s payload=%.*s qos=%u packet_id=%u\n",
               (int)msg->topic.len,
               msg->topic.buf,
               (int)msg->data.len,
               msg->data.buf,
               msg->qos,
               msg->id);
    } else if (event == MG_EV_MQTT_CMD) {
        struct mg_mqtt_message* msg = (struct mg_mqtt_message*)event_data;
        if (msg->cmd == MQTT_CMD_SUBACK) {
            printf("SUBACK packet_id=%u\n", msg->id);
        } else if (msg->cmd == MQTT_CMD_PUBACK) {
            printf("PUBACK packet_id=%u\n", msg->id);
        } else if (msg->cmd == MQTT_CMD_PINGRESP) {
            printf("PINGRESP\n");
        }
    } else if (event == MG_EV_ERROR) {
        printf("MQTT error: %s\n", (char*)event_data);
    } else if (event == MG_EV_CLOSE) {
        printf("MQTT connection closed\n");
        if (conn == s_mqtt_conn) {
            s_mqtt_conn = NULL;
            s_mqtt_ready = false;
        }
    }
}

static void publish_status(struct mg_connection* conn)
{
    char payload[128];
    snprintf(payload,
             sizeof(payload),
             "{\"device\":\"001\",\"online\":true,\"ts\":%ld}",
             (long)time(NULL));

    struct mg_mqtt_opts pub_opts;
    memset(&pub_opts, 0, sizeof(pub_opts));
    pub_opts.topic = mg_str(s_pub_topic);
    pub_opts.message = mg_str(payload);
    pub_opts.qos = 1;
    pub_opts.retain = false;
    pub_opts.retransmit_id = 0;

    mg_mqtt_pub(conn, &pub_opts);
    printf("PUB topic=%s payload=%s\n", s_pub_topic, payload);
}

int main(int argc, char* argv[])
{
    const char* broker_url = argc > 1 ? argv[1] : "mqtt://127.0.0.1:1883";

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    struct mg_mqtt_opts conn_opts;
    memset(&conn_opts, 0, sizeof(conn_opts));
    conn_opts.client_id = mg_str("mongoose-demo-client");
    conn_opts.keepalive = 30;
    conn_opts.clean = true;
    conn_opts.version = 4; // MQTT 3.1.1

    s_mqtt_conn = mg_mqtt_connect(&mgr, broker_url, &conn_opts, mqtt_event_handler, NULL);
    if (s_mqtt_conn == NULL) {
        printf("connect start failed: %s\n", broker_url);
        mg_mgr_free(&mgr);
        return 1;
    }

    printf("Connecting to %s\n", broker_url);
    printf("Press Ctrl+C to stop.\n");

    time_t last_pub = 0;
    while (s_running) {
        mg_mgr_poll(&mgr, 100);

        time_t now = time(NULL);
        if (s_mqtt_ready && s_mqtt_conn != NULL && now - last_pub >= 5) {
            publish_status(s_mqtt_conn);
            last_pub = now;
        }
    }

    if (s_mqtt_conn != NULL) {
        mg_mqtt_disconnect(s_mqtt_conn, NULL);
        mg_mgr_poll(&mgr, 100);
    }

    mg_mgr_free(&mgr);
    return 0;
}
