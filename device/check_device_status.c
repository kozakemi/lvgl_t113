/**
 * @file check_device_status.c
 * @brief 设备工作状态检查
 * @author Kozakemi (kemikoza@gmail.com)
 * @date 2025-02-14
 * @copyright Copyright (c) 2025  dongfang-wisdom
 * 
 * @par   功能描述
 * -# 检查设备的工作状态
 * @par   用法描述
 * -# `int check_wifi_state(char *interface)`检查wifi工作状态
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2025-02-14 <td>1.0     <td>wangh     <td>内容
 * </table>
 */
#include "check_device_status.h"
#include <stdio.h>

/**
 * @brief 检查WIFI的工作状态
 * 
 * 该函数通过读取系统文件来检查指定WIFI接口的工作状态。
 * 如果接口状态为"up"，则表示WIFI已连接；否则表示WIFI未连接。
 * 
 * @param {char *} interface WIFI接口名称
 * @return 0 表示WIFI已连接，1 表示WIFI未连接或读取失败
 */
int check_wifi_state(char *interface) {
    FILE *fp;
    char operstate[10];

    // 构建路径
    char path[50];
    snprintf(path, sizeof(path), "/sys/class/net/%s/operstate", interface);

    // 打开文件读取操作状态
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Failed to open operstate file");
        return 1;
    }

    // 读取状态
    if (fgets(operstate, sizeof(operstate), fp) != NULL) {
        if (strncmp(operstate, "up", 2) == 0) {
            printf("WiFi interface %s is up\n", interface);
            fclose(fp);
            return 0;
        } else {
            printf("WiFi interface %s is down\n", interface);
            fclose(fp);
            return 1;
        }
    } else {
        printf("Failed to read WiFi state\n");
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 1;
}