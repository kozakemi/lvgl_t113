#include "lvgl/lvgl.h"
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "calender_page.h"
#include "home_page.h"
#include "message_page.h"

#include "source/img/Bluetuth_OFF.h"
#include "source/img/Bluetuth_ON.h"
#include "source/img/calendar.h"
#include "source/img/comment.h"
#include "source/img/home.h"
#include "source/img/loading.h"
#include "source/img/mikuimg.h"
#include "source/img/No_Wifi.h"
#include "source/img/shezhi.h"
#include "source/img/Wifi-1.h"


#include "device/check_device_status.h"

typedef struct _lv_clock
{
    lv_obj_t *time_label;    // 时间标签
    lv_obj_t *date_label;    // 日期标签
    lv_obj_t *weekday_label; // 星期标签
} lv_clock_t;
typedef struct _lv_status_lable
{
    lv_obj_t *wifi_label; // WIFI标签
    lv_obj_t *blue_label; // 蓝牙标签
} lv_status_lable_t;
lv_obj_t *HomePage_OBJ;
static lv_coord_t hor_res = 480;
static lv_coord_t ver_res = 480;
static lv_coord_t rect_width =(lv_coord_t)((float)480 * 0.2f); // 矩形宽度
static void clock_date_task_callback(lv_timer_t *timer)
{
    static const char *week_day[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    static time_t unix_time;
    static struct tm *time_info;

    unix_time = time(NULL);
    time_info = localtime(&unix_time);

    int year = time_info->tm_year + 1900;
    int month = time_info->tm_mon + 1;
    int day = time_info->tm_mday;
    int weekday = time_info->tm_wday;
    int hour = time_info->tm_hour;
    int minutes = time_info->tm_min;
    int second = time_info->tm_sec;

    if (timer != NULL && timer->user_data != NULL)
    {
        lv_clock_t *clock = (lv_clock_t *)(timer->user_data);
        if (clock->time_label != NULL)
        {
            lv_label_set_text_fmt(clock->time_label, "%02d:%02d:%02d", hour, minutes, second);
            lv_obj_align(clock->time_label, LV_ALIGN_BOTTOM_LEFT, 230, -15);

            if (clock->date_label != NULL)
            {
                lv_label_set_text_fmt(clock->date_label, "%d-%02d-%02d", year, month, day);
                lv_obj_align(clock->date_label, LV_ALIGN_BOTTOM_MID, 50, -60);
            }

            if (clock->weekday_label != NULL)
            {
                lv_label_set_text_fmt(clock->weekday_label, "%s", week_day[weekday]);
                // lv_obj_align_to(clock->weekday_label, lv_obj_get_parent(clock->weekday_label), LV_ALIGN_BOTTOM_MID, -2, 0);
                lv_obj_align(clock->weekday_label, LV_ALIGN_BOTTOM_MID, -110, -20);
            }
        }
    }
}
static void clock_wb_status_task_callback(lv_timer_t *timer)
{
    if (timer != NULL && timer->user_data != NULL)
    {
        lv_status_lable_t *status_lable = (lv_clock_t *)(timer->user_data);
        if (status_lable->wifi_label != NULL)
        {

            if (0==check_wifi_state("wlan0"))
            {
                lv_img_set_src(status_lable->wifi_label, &Wifi_1);
            }
            else
            {
                lv_img_set_src(status_lable->wifi_label, &No_Wifi);
            }

            lv_obj_align(status_lable->wifi_label, LV_ALIGN_TOP_LEFT, (hor_res - rect_width) / 3, ver_res - ver_res * 0.28f);
        }
        if (status_lable->blue_label != NULL)
        {

            if (0==0)
            {
                lv_img_set_src(status_lable->blue_label, &Bluetuth_ON);
            }
            else
            {
                lv_img_set_src(status_lable->blue_label, &Bluetuth_OFF);
            }

            lv_obj_align(status_lable->blue_label, LV_ALIGN_TOP_LEFT, (hor_res - rect_width) / 3 * 2, ver_res - ver_res * 0.35f);
        }
    }
}

static void calender_img_clicked_callback(lv_event_t *e)
{
    LV_LOG_USER("Clicked");
    // lv_scr_load(Calender_OBJ);
    // if (Calender_OBJ != NULL) {
    //     lv_obj_del(Calender_OBJ);
    //     Calender_OBJ = NULL;  // 确保指针重置
    // }
    // Calender_OBJ = lv_obj_create(NULL);  // 创建新的页面对象
    lv_scr_load_anim(Calender_OBJ, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
}
static void message_img_clicked_callback(lv_event_t *e){
    LV_LOG_USER("Clicked");
    // lv_scr_load(MessageBox_OBJ);
    // if (MessageBox_OBJ != NULL) {
    //     lv_obj_del(MessageBox_OBJ);
    //     MessageBox_OBJ = NULL;  // 确保指针重置
    // }
    // MessageBox_OBJ = lv_obj_create(NULL);  // 创建新的页面对象
    lv_scr_load_anim(MessagePage_OBJ, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
}


void HomePage(void)
{
    // 创建主页对象
    HomePage_OBJ = lv_obj_create(NULL);
    lv_obj_set_size(HomePage_OBJ, LV_HOR_RES, LV_VER_RES);

    // 声明图片资源
    LV_IMG_DECLARE(mikuimg);

    // 创建一个用于显示图片的图片对象
    lv_obj_t *img = lv_img_create(HomePage_OBJ); // 创建在当前活动的屏幕
    lv_img_set_src(img, &mikuimg);
    lv_obj_align(img, LV_ALIGN_TOP_LEFT, 0, 0);

    // 底部装饰1
    lv_obj_t *canvas = lv_canvas_create(HomePage_OBJ);
    static lv_color_t canvasBuf[(32 * 480) / 8 * 480];
    lv_canvas_set_buffer(canvas, canvasBuf, 480, 480, LV_IMG_CF_TRUE_COLOR_ALPHA);
    const lv_point_t polygonPoint[4] = {
        {0, ver_res - ver_res * 0.1f},
        {hor_res - rect_width, ver_res - ver_res * 0.3f},
        {hor_res - rect_width, ver_res},
        {0, ver_res}
    };
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_hex(0x567c8c);
    lv_canvas_draw_polygon(canvas, polygonPoint, 4, &rect_dsc);

    // 底部装饰2
    lv_obj_t *canvas1 = lv_canvas_create(HomePage_OBJ);
    static lv_color_t canvasBuf1[(32 * 480) / 8 * 480];
    lv_canvas_set_buffer(canvas1, canvasBuf1, 480, 480, LV_IMG_CF_TRUE_COLOR_ALPHA);
    const lv_point_t polygonPoint1[4] = {
        {0, ver_res - ver_res * 0.25f},
        {hor_res - rect_width, ver_res - ver_res * 0.45f},
        {hor_res - rect_width, ver_res - ver_res * 0.3f},
        {0, ver_res - ver_res * 0.1f}
    };
    lv_draw_rect_dsc_t rect_dsc1;
    lv_draw_rect_dsc_init(&rect_dsc1);
    rect_dsc1.bg_color = lv_color_hex(0x000000);
    rect_dsc1.bg_opa = LV_OPA_80;
    lv_canvas_draw_polygon(canvas1, polygonPoint1, 4, &rect_dsc1);

    // 状态标签
    static lv_status_lable_t status_lable = {0};
    status_lable.wifi_label = lv_img_create(HomePage_OBJ);
    status_lable.blue_label = lv_img_create(HomePage_OBJ);
    lv_timer_t *wb_task_timer = lv_timer_create(clock_wb_status_task_callback, 1000, (void *)&status_lable); // 创建定时任务，1秒一次

    // 右侧栏
    static lv_style_t style_rect;
    lv_obj_t *rect;
    lv_color_t c = lv_color_hex(0xadc0c8);       // 修改颜色值
    lv_style_init(&style_rect);                  // 初始化样式
    lv_style_set_bg_color(&style_rect, c);       // 设置颜色
    lv_style_set_width(&style_rect, rect_width); // 设置宽度
    lv_style_set_height(&style_rect, ver_res);   // 设置高度
    lv_style_set_radius(&style_rect, 0);         // 设置圆角
    lv_style_set_shadow_width(&style_rect, 25);  // 设置阴影宽度
    lv_style_set_shadow_ofs_x(&style_rect, -3);  // 设置水平偏移
    lv_style_set_border_width(&style_rect, 0);   // 设置边框宽度
    rect = lv_obj_create(HomePage_OBJ);
    lv_obj_align(rect, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_style(rect, &style_rect, 0);

    // 创建设置图片按钮
    lv_obj_t *shezhi_img = lv_img_create(HomePage_OBJ); // 创建在当前活动的屏幕
    lv_img_set_src(shezhi_img, &shezhi);
    lv_obj_align(shezhi_img, LV_ALIGN_TOP_RIGHT, -1 * rect_width / 2.0 + 64 / 2, 1 * ver_res / 5.0 - 64 / 2);

    // 创建日历图片按钮
    lv_obj_t *calendar_img_btn = lv_imgbtn_create(HomePage_OBJ); // 创建在当前活动的屏幕
    lv_imgbtn_set_src(calendar_img_btn, LV_IMGBTN_STATE_RELEASED, &calendar1, NULL, NULL);
    lv_obj_align(calendar_img_btn, LV_ALIGN_TOP_RIGHT, -1 * rect_width / 2.0 + 64 / 2, 2 * ver_res / 5.0 - 64 / 2);
    lv_obj_set_size(calendar_img_btn, 64, 64);
    lv_obj_add_event_cb(calendar_img_btn, calender_img_clicked_callback, LV_EVENT_CLICKED, NULL);

    // 创建消息图片按钮
    lv_obj_t *message_img_btn = lv_imgbtn_create(HomePage_OBJ); // 创建在当前活动的屏幕
    lv_imgbtn_set_src(message_img_btn, LV_IMGBTN_STATE_RELEASED, &comment, NULL, NULL);
    lv_obj_set_size(message_img_btn, 64, 64);
    lv_obj_align(message_img_btn, LV_ALIGN_TOP_RIGHT, -1 * rect_width / 2.0 + 64 / 2, 3 * ver_res / 5.0 - 64 / 2);
    lv_obj_add_event_cb(message_img_btn, message_img_clicked_callback, LV_EVENT_CLICKED, NULL);

    // 创建加载图片
    lv_obj_t *loading_img = lv_img_create(HomePage_OBJ); // 创建在当前活动的屏幕
    lv_img_set_src(loading_img, &loading);
    lv_obj_align(loading_img, LV_ALIGN_TOP_RIGHT, -1 * rect_width / 2.0 + 64 / 2, 4 * ver_res / 5.0 - 64 / 2);

    // 时间标签样式
    static lv_style_t time_label_style;
    lv_style_init(&time_label_style);
    lv_style_set_text_color(&time_label_style, lv_color_white());
    lv_style_set_text_font(&time_label_style, &lv_font_montserrat_32);
    lv_style_set_text_opa(&time_label_style, LV_OPA_COVER);
    lv_style_set_bg_opa(&time_label_style, LV_OPA_0);
    lv_style_set_text_color(&time_label_style, lv_color_hex(0xdba7af));

    // 日期标签样式
    static lv_style_t date_label_style;
    lv_style_init(&date_label_style);
    lv_style_set_text_opa(&date_label_style, LV_OPA_COVER);
    lv_style_set_bg_opa(&date_label_style, LV_OPA_0);
    lv_style_set_text_color(&date_label_style, lv_color_white());
    lv_style_set_text_font(&date_label_style, &lv_font_montserrat_20);

    // 星期标签样式
    static lv_style_t week_lable_style;
    lv_style_init(&week_lable_style);
    lv_style_set_text_opa(&week_lable_style, LV_OPA_COVER);
    lv_style_set_bg_opa(&week_lable_style, LV_OPA_0);
    lv_style_set_text_color(&week_lable_style, lv_color_white());
    lv_style_set_text_font(&week_lable_style, &lv_font_montserrat_26);

    // 创建时钟对象
    static lv_clock_t lv_clock = {0};
    lv_clock.time_label = lv_label_create(HomePage_OBJ);
    lv_obj_add_style(lv_clock.time_label, &time_label_style, LV_STATE_DEFAULT);

    lv_clock.date_label = lv_label_create(HomePage_OBJ);
    lv_obj_add_style(lv_clock.date_label, &date_label_style, LV_STATE_DEFAULT);

    lv_clock.weekday_label = lv_label_create(HomePage_OBJ);
    lv_obj_add_style(lv_clock.weekday_label, &week_lable_style, LV_STATE_DEFAULT);

    // 创建定时任务，200ms刷新一次
    lv_timer_t *task_timer = lv_timer_create(clock_date_task_callback, 200, (void *)&lv_clock);
}