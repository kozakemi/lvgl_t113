#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

// 定义全局变量
struct mosquitto *mqtt = NULL;
bool is_connected = false;
pthread_t mqtt_thread_id;
#define MQTT_USE_ID      "703e13100a3d43b6b4abbb628b3c3d89"
#define MQTT_BROKER_ADDR "119.91.109.180"
#define MQTT_BROKER_PORT  9501

// 回调函数：连接成功
static void on_connect_callback(struct mosquitto *mqtt, void *obj, int reason_code) {
    if (reason_code != 0) {
        fprintf(stderr, "Failed to connect to broker, reason code: %d\n", reason_code);
        exit(1);
    }
    printf("Connected to broker.\n");
    is_connected = true;
    mosquitto_subscribe(mqtt, NULL, "switch006", 0);
}

// 回调函数：接收消息
static void on_message_callback(struct mosquitto *mqtt, void *obj, const struct mosquitto_message *msg) {
    if (msg != NULL) {
        printf("Received message on topic '%s': %s\n", msg->topic, (char *)msg->payload);
    }
}

// 回调函数：连接断开
static void on_disconnect_callback(struct mosquitto *mqtt, void *obj, int reason_code) {
    printf("Disconnected from broker, reason code: %d\n", reason_code);
    is_connected = false;
}


// 初始化 MQTT 客户端
static int mqtt_init() {
    int ret;

    mosquitto_lib_init();
    mqtt = mosquitto_new(MQTT_USE_ID, true, NULL);
    if (!mqtt) {
        fprintf(stderr, "Failed to create mosquitto instance.\n");
        return -1;
    }

    mosquitto_connect_callback_set(mqtt, on_connect_callback);
    mosquitto_message_callback_set(mqtt, on_message_callback);
    mosquitto_disconnect_callback_set(mqtt, on_disconnect_callback);

    ret = mosquitto_connect(mqtt, MQTT_BROKER_ADDR, MQTT_BROKER_PORT, 60);
    if (ret != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Failed to connect to broker, error: %d\n", ret);
        mosquitto_destroy(mqtt);
        return -1;
    }

    return 0;
}


// MQTT 客户端线程函数
static void *mqtt_thread(void *arg) {
    while (1) {
        if (mqtt != NULL) {
            mosquitto_loop(mqtt, 1000, 1);
        }
        sleep(1);
    }
    return NULL;
}

// 检查 MQTT 连接状态
bool mqtt_is_connected() {
    return is_connected;
}


// 启动 MQTT 客户端
int start_mqtt_client() {
    // 初始化 MQTT 客户端
    if (mqtt_init() != 0) {
        fprintf(stderr, "Failed to initialize MQTT client.\n");
        return -1;
    }

    // 创建 MQTT 客户端线程
    if (pthread_create(&mqtt_thread_id, NULL, mqtt_thread, NULL) != 0) {
        fprintf(stderr, "Failed to create MQTT thread.\n");
        mosquitto_destroy(mqtt);
        mosquitto_lib_cleanup();
        return -1;
    }

    return 0;
}

// 停止 MQTT 客户端
void stop_mqtt_client() {
    if (mqtt != NULL) {
        mosquitto_destroy(mqtt);
    }
    mosquitto_lib_cleanup();
    pthread_cancel(mqtt_thread_id);
}