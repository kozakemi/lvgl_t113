/**@mainpage  T113-S3 86的LVGL桌面时钟程序
* <table>
* <tr><th>Project  <td>lvgl_t113 
* <tr><th>Author   <td>Kozakemi 
* </table>
* @section   项目详细描述
* 显示日期,时间,拥有MQTT Client功能(详细功能未开发,保留接口)
*
* @section   功能描述  
* -# lvgl 8.3.10
* -# MQTT mosquitto
* 
* @section   用法描述 
* -# cd build && ./build_lv_taiji
* -# adb push lv_taiji /{path}
* -# sh\> lv_taiji 5
* 
* @section   固件更新 
* <table>
* <tr><th>Date        <th>H_Version  <th>S_Version  <th>Author    <th>Description  </tr>
* <tr><td>2025/02/14  <td>1.0        <td>0.1        <td>Kozakemi  <td>创建初始版本 </tr>
* </tr>
* </table>
**********************************************************************************
*/


/**
 * @file main.c
 * @brief 主程序入口文件，初始化LVGL库并配置显示和输入设备驱动，提供不同演示模式的选择。
 * @author Kozakemi (kemikoza@gmail.com)
 * @date 2025-02-14
 * @copyright Copyright (c) 2025 Kozakemi
 * 
 * @par 功能描述
 * -# 初始化LVGL库
 * -# 配置显示驱动（sunxifb）
 * -# 配置输入设备驱动（evdev）
 * -# 提供命令行参数选择不同的演示模式或启动自定义页面（主页、日历页、消息页）
 * -# 启动MQTT客户端用于网络通信
 * 
 * @par 用法描述
 * -# 编译并运行此程序时，可以通过命令行参数选择不同的演示模式：
 *   - lv_examples 0: lv_demo_widgets
 *   - lv_examples 1: lv_demo_music
 *   - lv_examples 2: lv_demo_benchmark
 *   - lv_examples 3: lv_demo_keypad_encoder
 *   - lv_examples 4: lv_demo_stress
 *   - lv_examples 5: 启动自定义页面（主页、日历页、消息页）并连接MQTT服务器
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2025-02-14 <td>1.0     <td>Kozakemi<td>初始版本，实现基本功能
 * </table>
 */
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/sunxifb.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include "lv_font.h"

#include "page/home_page.h"
#include "page/calender_page.h"
#include "page/message_page.h"

#include "device/mqtt/mqtt_client.h"

static void hal_init(void);
static int tick_thread(void *data);



/**
 * @brief 主程序入口函数
 * 
 * @param {int} argc 
 * @param {char *} argv 
 * @return {int}
 */
int main(int argc, char *argv[])
{
    if (argv[1] == NULL || atoi(argv[1]) < 0 || atoi(argv[1]) > 5) {
        printf("lv_examples 0, is lv_demo_widgets\n");
        printf("lv_examples 1, is lv_demo_music\n");
        printf("lv_examples 2, is lv_demo_benchmark\n");
        printf("lv_examples 3, is lv_demo_keypad_encoder\n");
        printf("lv_examples 4, is lv_demo_stress\n");
        return 0;
    }

    /*LittlevGL init*/
    lv_init();

    uint32_t rotated = LV_DISP_ROT_NONE;

    /*Linux frame buffer device init*/
    sunxifb_init(rotated);

    /*A buffer for LittlevGL to draw the screen's content*/
    static uint32_t width, height;
    sunxifb_get_sizes(&width, &height);

    static lv_color_t *buf;
    buf = (lv_color_t*) sunxifb_alloc(width * height * sizeof(lv_color_t),
            "lv_examples");

    if (buf == NULL) {
        sunxifb_exit();
        printf("malloc draw buffer fail\n");
        return 0;
    }

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, width * height);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = sunxifb_flush;
    disp_drv.hor_res    = width;
    disp_drv.ver_res    = height;
    disp_drv.rotated    = rotated;
#ifndef USE_SUNXIFB_G2D_ROTATE
    if (rotated != LV_DISP_ROT_NONE)
        disp_drv.sw_rotate = 1;
#endif
    lv_disp_drv_register(&disp_drv);

    evdev_init();
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);                /*Basic initialization*/
    indev_drv.type =LV_INDEV_TYPE_POINTER;        /*See below.*/
    indev_drv.read_cb = evdev_read;               /*See below.*/
    /*Register the driver in LVGL and save the created input device object*/
    lv_indev_t * evdev_indev = lv_indev_drv_register(&indev_drv);

    /*Create a Demo*/
    switch(atoi(argv[1])) {
    case 0:
        lv_demo_widgets();
        break;
    case 1:
        lv_demo_music();
        break;
    case 2:
        lv_demo_benchmark();
        break;
    case 3:
        lv_demo_keypad_encoder();
        break;
    case 4:
        lv_demo_stress();
        break;
    case 5:
        start_mqtt_client();
        HomePage();
        CalenderPage();  
        MessagePage();
        lv_scr_load(HomePage_OBJ);
        break;
    default:
        free(buf);
        sunxifb_exit();
        return 0;
    }

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_task_handler();
        usleep(5000);
    }

    /*sunxifb_free((void**) &buf, "lv_examples");*/
    /*sunxifb_exit();*/
    return 0;
}


/**
 * @brief LVGL心跳接口 
 * 
 * @return {uint32_t}
 */
uint32_t custom_tick_get(void) {
    static uint64_t start_ms = 0;
    if (start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = ((uint64_t) tv_start.tv_sec * 1000000
                + (uint64_t) tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = ((uint64_t) tv_now.tv_sec * 1000000 + (uint64_t) tv_now.tv_usec)
            / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
