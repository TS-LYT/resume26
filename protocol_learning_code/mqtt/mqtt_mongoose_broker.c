#include "mongoose.h"

#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 学习用 MQTT broker：
// - 支持 CONNECT / SUBSCRIBE / PUBLISH / PINGREQ / DISCONNECT。
// - 支持主题通配符 + 和 #。
// - 只演示 QoS0/QoS1 的基本转发，不实现持久会话、离线消息、retain 存储和完整 QoS2。

struct subscription {
    struct subscription* next;
    struct mg_connection* conn;
    char* topic;
    size_t topic_len;
    uint8_t qos;
};

static volatile sig_atomic_t s_running = 1;
static struct subscription* s_subs = NULL;

static void signal_handler(int signo)
{
    (void)signo;
    s_running = 0;
}

static char* dup_mg_str(struct mg_str str)
{
    char* out = (char*)calloc(str.len + 1, 1);
    if (out != NULL && str.len > 0) {
        memcpy(out, str.buf, str.len);
    }
    return out;
}

static bool topic_filter_match(const char* filter, const char* topic)
{
    const char* fp = filter;
    const char* tp = topic;

    while (*fp != '\0') {
        if (*fp == '#') {
            return fp[1] == '\0';
        }

        if (*fp == '+') {
            while (*tp != '\0' && *tp != '/') {
                tp++;
            }
            fp++;
        } else {
            if (*tp == '\0' || *fp != *tp) {
                return false;
            }
            fp++;
            tp++;
        }

        if (*fp == '/' && *tp == '/') {
            fp++;
            tp++;
        } else if (*fp == '\0' && *tp == '\0') {
            return true;
        } else if (*fp == '\0' || *tp == '\0') {
            return *fp == '#' && fp[1] == '\0';
        }
    }

    return *tp == '\0';
}

static size_t next_sub_topic(struct mg_mqtt_message* msg,
                             struct mg_str* topic,
                             uint8_t* qos,
                             size_t pos)
{
    const uint8_t* buf = (const uint8_t*)msg->dgram.buf;

    if (pos + 2 > msg->dgram.len) {
        return 0;
    }

    size_t topic_len = ((size_t)buf[pos] << 8) | buf[pos + 1];
    size_t next = pos + 2 + topic_len + 1;
    if (next > msg->dgram.len) {
        return 0;
    }

    topic->buf = (char*)buf + pos + 2;
    topic->len = topic_len;
    *qos = buf[pos + 2 + topic_len] & 0x03;
    return next;
}

static void remove_subscriptions(struct mg_connection* conn)
{
    struct subscription** pp = &s_subs;
    while (*pp != NULL) {
        struct subscription* sub = *pp;
        if (sub->conn == conn) {
            *pp = sub->next;
            printf("UNSUB conn=%p topic=%s\n", (void*)conn, sub->topic);
            free(sub->topic);
            free(sub);
        } else {
            pp = &sub->next;
        }
    }
}

static void handle_connect(struct mg_connection* conn, struct mg_mqtt_message* msg)
{
    if (msg->dgram.len < 10) {
        mg_error(conn, "malformed CONNECT");
        return;
    }

    if ((uint8_t)msg->dgram.buf[8] != 4) {
        mg_error(conn, "only MQTT 3.1.1 is supported in this demo");
        return;
    }

    uint8_t response[] = {0x00, 0x00}; // Session Present=0, Return Code=0
    mg_mqtt_send_header(conn, MQTT_CMD_CONNACK, 0, sizeof(response));
    mg_send(conn, response, sizeof(response));
    printf("CONNECT conn=%p accepted\n", (void*)conn);
}

static void handle_subscribe(struct mg_connection* conn, struct mg_mqtt_message* msg)
{
    size_t pos = 4; // fixed header + packet id 后是 topic filter 列表。
    uint8_t granted_qos[128];
    size_t count = 0;
    struct mg_str topic;
    uint8_t qos = 0;

    while (count < sizeof(granted_qos) && (pos = next_sub_topic(msg, &topic, &qos, pos)) > 0) {
        struct subscription* sub = (struct subscription*)calloc(1, sizeof(*sub));
        if (sub == NULL) {
            break;
        }

        sub->conn = conn;
        sub->topic = dup_mg_str(topic);
        sub->topic_len = topic.len;
        sub->qos = qos > 1 ? 1 : qos;
        sub->next = s_subs;
        s_subs = sub;

        granted_qos[count++] = sub->qos;
        printf("SUB conn=%p topic=%s qos=%u\n", (void*)conn, sub->topic, sub->qos);
    }

    mg_mqtt_send_header(conn, MQTT_CMD_SUBACK, 0, (uint32_t)(2 + count));
    uint16_t packet_id = mg_htons(msg->id);
    mg_send(conn, &packet_id, sizeof(packet_id));
    mg_send(conn, granted_qos, count);
}

static void handle_publish(struct mg_connection* conn, struct mg_mqtt_message* msg)
{
    char* topic = dup_mg_str(msg->topic);
    if (topic == NULL) {
        return;
    }

    printf("PUB conn=%p topic=%s payload=%.*s qos=%u\n",
           (void*)conn,
           topic,
           (int)msg->data.len,
           msg->data.buf,
           msg->qos);

    for (struct subscription* sub = s_subs; sub != NULL; sub = sub->next) {
        if (topic_filter_match(sub->topic, topic)) {
            struct mg_mqtt_opts pub_opts;
            memset(&pub_opts, 0, sizeof(pub_opts));
            pub_opts.topic = msg->topic;
            pub_opts.message = msg->data;
            pub_opts.qos = sub->qos;
            pub_opts.retain = false;
            pub_opts.retransmit_id = 0;
            mg_mqtt_pub(sub->conn, &pub_opts);
        }
    }

    free(topic);
}

static void mqtt_broker_handler(struct mg_connection* conn, int event, void* event_data)
{
    if (event == MG_EV_MQTT_CMD) {
        struct mg_mqtt_message* msg = (struct mg_mqtt_message*)event_data;

        switch (msg->cmd) {
        case MQTT_CMD_CONNECT:
            handle_connect(conn, msg);
            break;
        case MQTT_CMD_SUBSCRIBE:
            handle_subscribe(conn, msg);
            break;
        case MQTT_CMD_PUBLISH:
            handle_publish(conn, msg);
            break;
        case MQTT_CMD_PINGREQ:
            mg_mqtt_send_header(conn, MQTT_CMD_PINGRESP, 0, 0);
            break;
        case MQTT_CMD_DISCONNECT:
            conn->is_closing = 1;
            break;
        default:
            printf("MQTT cmd=%u ignored in demo broker\n", msg->cmd);
            break;
        }
    } else if (event == MG_EV_CLOSE) {
        remove_subscriptions(conn);
    }
}

int main(int argc, char* argv[])
{
    const char* listen_url = argc > 1 ? argv[1] : "mqtt://0.0.0.0:1883";

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    if (mg_mqtt_listen(&mgr, listen_url, mqtt_broker_handler, NULL) == NULL) {
        printf("listen failed: %s\n", listen_url);
        mg_mgr_free(&mgr);
        return 1;
    }

    printf("Mongoose demo MQTT broker listening on %s\n", listen_url);
    printf("Press Ctrl+C to stop.\n");

    while (s_running) {
        mg_mgr_poll(&mgr, 1000);
    }

    mg_mgr_free(&mgr);
    return 0;
}
