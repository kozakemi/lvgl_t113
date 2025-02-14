/**
 * @file mqtt_client.c
 * @brief 实现MQTT客户端的功能，包括初始化、连接、消息接收和断开连接等操作。
 * @author Kozakemi (kemikoza@gmail.com)
 * @date 2025-02-14
 * @copyright Copyright (c) 2025 Kozakemi
 * 
 * @par 功能描述
 * -# 初始化MQTT客户端。
 * -# 连接到MQTT代理。
 * -# 设置回调函数以处理连接、消息接收和断开连接事件。
 * -# 启动一个线程来处理MQTT消息循环。
 * -# 提供启动和停止MQTT客户端的接口。
 * 
 * @par 用法描述
 * -# 调用 `start_mqtt_client()` 函数启动MQTT客户端。
 * -# 调用 `stop_mqtt_client()` 函数停止MQTT客户端。
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2025-02-14 <td>1.0     <td>Kozakemi  <td>初始版本，实现MQTT客户端的基本功能。
 * </table>
 */

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

/**
 * @brief  连接成功回调
 * 
 * @param {struct mosquitto *} mqtt 
 * @param {void *} obj 
 * @param {int} reason_code 
 */
static void on_connect_callback(struct mosquitto *mqtt, void *obj, int reason_code) {
    if (reason_code != 0) {
        fprintf(stderr, "Failed to connect to broker, reason code: %d\n", reason_code);
        exit(1);
    }
    printf("Connected to broker.\n");
    is_connected = true;
    mosquitto_subscribe(mqtt, NULL, "switch006", 0);
}

/**
 * @brief 接收消息回调
 * 
 * @param {struct mosquitto *} mqtt 
 * @param {void *} obj 
 * @param {const struct mosquitto_message *} msg 
 */
static void on_message_callback(struct mosquitto *mqtt, void *obj, const struct mosquitto_message *msg) {
    if (msg != NULL) {
        printf("Received message on topic '%s': %s\n", msg->topic, (char *)msg->payload);
    }
}

/**
 * @brief 连接短肽回调
 * 
 * @param {struct mosquitto *} mqtt 
 * @param {void *} obj 
 * @param {int} reason_code 
 */
static void on_disconnect_callback(struct mosquitto *mqtt, void *obj, int reason_code) {
    printf("Disconnected from broker, reason code: %d\n", reason_code);
    is_connected = false;
}


/**
 * @brief 初始化MQTT客户端。
 * 
 * @return int 成功返回0，失败返回-1。
 */
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

/**
 * @brief mqtt线程
 * 
 * @param {type} arg 
 * @return {void*}
 */
 */
static void *mqtt_thread(void *arg) {
    while (1) {
        if (mqtt != NULL) {
            mosquitto_loop(mqtt, 1000, 1);
        }
        sleep(1);
    }
    return NULL;
}

/**
 * @brief mqtt连接状态
 * 
 * @return {true}
 * @return {false}
 */
bool mqtt_is_connected() {
    return is_connected;
}


/**
 * @brief 启动MQTT客户端。
 * 
 * @return int 成功返回0，失败返回-1。
 */
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

/**
 * @brief 停止MQTT客户端。
 */
void stop_mqtt_client() {
    if (mqtt != NULL) {
        mosquitto_destroy(mqtt);
    }
    mosquitto_lib_cleanup();
    pthread_cancel(mqtt_thread_id);
}