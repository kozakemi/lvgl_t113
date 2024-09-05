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
#include "mikuimg.h"
#include "shezhi.h"
#include "calendar.h"
#include "comment.h"
#include "loading.h"
#include "Wifi-1.h"
#include "No_Wifi.h"
#include "Bluetuth_OFF.h"
#include "Bluetuth_ON.h"
#include "check_device_status.h"
#include "home.h"
static void hal_init(void);
static int tick_thread(void *data);
static lv_obj_t *HomePage_OBJ;
static lv_obj_t *Calender_OBJ;
static lv_obj_t *two;
#define LV_COLOR_RED LV_COLOR_MAKE32(0xFF, 0x00, 0x00)
lv_coord_t hor_res = 480;
lv_coord_t ver_res = 480;
lv_coord_t rect_width =(lv_coord_t)((float)480 * 0.2f); // 矩形宽度
void my_display(void); 
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
void lvgl_calendar_show_data_test(void)
{
    lv_calendar_date_t today;
    lv_calendar_date_t *today_get;
    today.year = 2021;
    today.month = 2;
    today.day = 20;

    lv_obj_t *calendar = lv_calendar_create(lv_scr_act());
    lv_obj_set_size(calendar, 300, 300);
    lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 0);

    lv_calendar_set_today_date(calendar, today.year, today.month, today.day);
    lv_calendar_set_showed_date(calendar, today.year, today.month);
    today_get = lv_calendar_get_today_date(calendar);
    printf("Y/M/D:%04d/%02d/%02d\n", today_get->year, today_get->month, today_get->day);
}
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

void HomePage(void)
{

    HomePage_OBJ = lv_obj_create(NULL);
    lv_obj_set_size(HomePage_OBJ, LV_HOR_RES, LV_VER_RES);
    // 声明图片资源
    LV_IMG_DECLARE(mikuimg); // 确保mikuimg.c和mikuimg.h文件已经生成
    /*miku IMG*/
    // 创建一个用于显示图片的图片对象
    lv_obj_t *img = lv_img_create(HomePage_OBJ); // 创建在当前活动的屏幕

    lv_img_set_src(img, &mikuimg);
    lv_obj_align(img, LV_ALIGN_TOP_LEFT, 0, 0);

    /*底部装饰1*/
    lv_obj_t *canvas = lv_canvas_create(HomePage_OBJ);
    static lv_color_t canvasBuf[(32 * 480) / 8 * 480];
    lv_canvas_set_buffer(canvas, canvasBuf, 480, 480, LV_IMG_CF_TRUE_COLOR_ALPHA);
    const lv_point_t polygonPoint[4] = {{0, ver_res - ver_res * 0.1f}, {hor_res - rect_width, ver_res - ver_res * 0.3f}, {hor_res - rect_width, ver_res}, {0, ver_res}};
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_hex(0x567c8c);
    // rect_dsc.bg_opa=LV_OPA_10;
    lv_canvas_draw_polygon(canvas, polygonPoint, 4, &rect_dsc);

    /*底部装饰2*/
    lv_obj_t *canvas1 = lv_canvas_create(HomePage_OBJ);
    static lv_color_t canvasBuf1[(32 * 480) / 8 * 480];
    lv_canvas_set_buffer(canvas1, canvasBuf1, 480, 480, LV_IMG_CF_TRUE_COLOR_ALPHA);
    const lv_point_t polygonPoint1[4] = {{0, ver_res - ver_res * 0.25f}, {hor_res - rect_width, ver_res - ver_res * 0.45f}, {hor_res - rect_width, ver_res - ver_res * 0.3f}, {0, ver_res - ver_res * 0.1f}};
    lv_draw_rect_dsc_t rect_dsc1;
    lv_draw_rect_dsc_init(&rect_dsc1);
    rect_dsc1.bg_color = lv_color_hex(0x000000);
    rect_dsc1.bg_opa = LV_OPA_80;
    lv_canvas_draw_polygon(canvas1, polygonPoint1, 4, &rect_dsc1);

    static lv_status_lable_t status_lable = {0};
    status_lable.wifi_label = lv_img_create(HomePage_OBJ);
    status_lable.blue_label = lv_img_create(HomePage_OBJ);
    lv_timer_t *wb_task_timer = lv_timer_create(clock_wb_status_task_callback, 1000, (void *)&status_lable); // 创建定时任务，1秒一次
    // lv_obj_t * blu_obj = lv_img_create(HomePage_OBJ); // 创建在当前活动的屏幕
    // lv_img_set_src(blu_obj, &Bluetuth_ON);
    // lv_img_set_angle(wifi_1_obj,3600-120);

    /*右侧栏*/
    static lv_style_t style_rect;
    lv_obj_t *rect;
    lv_color_t c = lv_color_hex(0xadc0c8);       // 修改颜色值
    lv_style_init(&style_rect);                  // 初始化样式
    lv_style_set_bg_color(&style_rect, c);       // 设置颜色
    lv_style_set_width(&style_rect, rect_width); // 设置宽度
    lv_style_set_height(&style_rect, ver_res);   // 设置高度
    lv_style_set_radius(&style_rect, 0);         // 设置圆角
    // lv_style_set_opa(&style_rect,LV_OPA_COVER); //设置透明度
    lv_style_set_shadow_width(&style_rect, 25); // 设置阴影宽度
    lv_style_set_shadow_ofs_x(&style_rect, -3); // 设置水平偏移
    // lv_style_set_shadow_color(&style_rect,lv_palette_main(LV_PALETTE_NONE));//设置阴影颜色
    lv_style_set_border_width(&style_rect, 0); // 设置边框宽度
    // lv_style_set_opa(&style_rect,LV_OPA_90);
    // lv_obj_set_flex_flow(HomePage_OBJ, LV_FLEX_FLOW_ROW);
    // lv_obj_set_flex_align(HomePage_OBJ, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    rect = lv_obj_create(HomePage_OBJ);
    lv_obj_align(rect, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_style(rect, &style_rect, 0);

    // 创建一个用于显示图片的图片对象
    lv_obj_t *shezhi_img = lv_img_create(HomePage_OBJ); // 创建在当前活动的屏幕
    lv_img_set_src(shezhi_img, &shezhi);
    lv_obj_align(shezhi_img, LV_ALIGN_TOP_RIGHT, -1 * rect_width / 2.0 + 64 / 2, 1 * ver_res / 5.0 - 64 / 2);

    // // 创建一个用于显示图片的图片对象
    // lv_obj_t * calendar_img = lv_img_create(HomePage_OBJ); // 创建在当前活动的屏幕
    // lv_img_set_src(calendar_img, &calendar);
    // lv_obj_align(calendar_img, LV_ALIGN_TOP_RIGHT, -1*rect_width/2.0+64/2, 2*ver_res/5.0-64/2);
    // lv_obj_add_event_cb(calendar_img, calender_img_clicked_callback, LV_EVENT_CLICKED, NULL);

    lv_obj_t *calendar_img_btn = lv_imgbtn_create(HomePage_OBJ); // 创建在当前活动的屏幕
    // 设置正常状态下的图片
    lv_imgbtn_set_src(calendar_img_btn, LV_IMGBTN_STATE_RELEASED, &calendar, NULL, NULL);
    // lv_imgbtn_set_src(calendar_img_btn, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &calendar, NULL);
    lv_obj_align(calendar_img_btn, LV_ALIGN_TOP_RIGHT, -1 * rect_width / 2.0 + 64 / 2, 2 * ver_res / 5.0 - 64 / 2);
    lv_obj_set_size(calendar_img_btn, 64, 64);
    lv_obj_add_event_cb(calendar_img_btn, calender_img_clicked_callback, LV_EVENT_CLICKED, NULL);

    // lv_obj_t * btn = lv_btn_create(HomePage_OBJ);
    // lv_obj_set_size(btn, 100, 50);
    // lv_obj_center(btn);
    // lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
    // lv_obj_t * label = lv_label_create(btn);
    // lv_label_set_text(label, "Click me!");
    // lv_obj_center(label);

    // 创建一个用于显示图片的图片对象
    lv_obj_t *comment_img = lv_img_create(HomePage_OBJ); // 创建在当前活动的屏幕
    lv_img_set_src(comment_img, &comment);
    lv_obj_align(comment_img, LV_ALIGN_TOP_RIGHT, -1 * rect_width / 2.0 + 64 / 2, 3 * ver_res / 5.0 - 64 / 2);

    // 创建一个用于显示图片的图片对象
    lv_obj_t *loading_img = lv_img_create(HomePage_OBJ); // 创建在当前活动的屏幕
    lv_img_set_src(loading_img, &loading);
    lv_obj_align(loading_img, LV_ALIGN_TOP_RIGHT, -1 * rect_width / 2.0 + 64 / 2, 4 * ver_res / 5.0 - 64 / 2);

    //////////////////////////////////////////////////////////////////////////////////////////

    /* Time font */
    static lv_style_t time_label_style;                                // 时间标签样式
    lv_style_init(&time_label_style);                                  // 初始化样式
    lv_style_set_text_color(&time_label_style, lv_color_white());      // 设置标签样式文本颜色
    lv_style_set_text_font(&time_label_style, &lv_font_montserrat_32); // 设置字体风格
    lv_style_set_text_opa(&time_label_style, LV_OPA_COVER);            // 设置字体透明度
    lv_style_set_bg_opa(&time_label_style, LV_OPA_0);                  // 设置样式背景透明度
    lv_style_set_text_color(&time_label_style, lv_color_hex(0xdba7af));

    // lv_style_set_transform_angle(&time_label_style, 300);
    /* Date font */
    static lv_style_t date_label_style; // 日期标签样式
    lv_style_init(&date_label_style);
    lv_style_set_text_opa(&date_label_style, LV_OPA_COVER);
    lv_style_set_bg_opa(&date_label_style, LV_OPA_0);
    lv_style_set_text_color(&date_label_style, lv_color_white());
    lv_style_set_text_font(&date_label_style, &lv_font_montserrat_20);
    // lv_style_set_transform_angle(&date_label_style, 300);
    /* Week font */
    static lv_style_t week_lable_style; // 日期标签样式
    lv_style_init(&week_lable_style);
    lv_style_set_text_opa(&week_lable_style, LV_OPA_COVER);
    lv_style_set_bg_opa(&week_lable_style, LV_OPA_0);
    lv_style_set_text_color(&week_lable_style, lv_color_white());

    // lv_style_set_transform_angle(&week_lable_style, 900);
    lv_style_set_text_font(&week_lable_style, &lv_font_montserrat_26);

    static lv_clock_t lv_clock = {0};

    lv_clock.time_label = lv_label_create(HomePage_OBJ);                        // 基于time_obj对象创建时间显示标签对象 lv_clock.time_label
    lv_obj_add_style(lv_clock.time_label, &time_label_style, LV_STATE_DEFAULT); // 给对象 lv_clock.time_label添加样式

    lv_clock.date_label = lv_label_create(HomePage_OBJ);                        // 基于date_obj对象创建lv_clock.date_label日期显示对象
    lv_obj_add_style(lv_clock.date_label, &date_label_style, LV_STATE_DEFAULT); // 给lv_clock.date_label对象添加样式

    /*Week display*/
    lv_clock.weekday_label = lv_label_create(HomePage_OBJ);                        // 基于date_obj对象创建星期显示lv_clock.weekday_label对象
    lv_obj_add_style(lv_clock.weekday_label, &week_lable_style, LV_STATE_DEFAULT); // 给对象lv_clock.weekday_label添加样式

    lv_timer_t *task_timer = lv_timer_create(clock_date_task_callback, 200, (void *)&lv_clock); // 创建定时任务，200ms刷新一次
}
static void event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        lv_calendar_date_t date;
        if (lv_calendar_get_pressed_date(obj, &date))
        {
            LV_LOG_USER("Clicked date: %02d.%02d.%d", date.day, date.month, date.year);
        }
    }
}
void home_img_clicked_callback(lv_event_t *e){
    LV_LOG_USER("Clicked");
    // lv_scr_load(HomePage_OBJ);
    lv_scr_load_anim(HomePage_OBJ, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
}
static void clock_calender_task_callback(lv_timer_t *timer){
    static time_t unix_time;
    static struct tm *time_info;
    unix_time = time(NULL);
    time_info = localtime(&unix_time);
    uint32_t year = time_info->tm_year + 1900;
    uint32_t month = time_info->tm_mon + 1;
    uint32_t day = time_info->tm_mday;
    printf("year:%d,month:%d,day:%d\n", year, month, day);
    lv_obj_t *calendar_obj=(lv_obj_t *)(timer->user_data);
    lv_calendar_set_today_date(calendar_obj, year, month, day);
    lv_calendar_set_showed_date(calendar_obj, year, month);
    printf("set date\n");
}

void CalenderPage(void)
{
    
    static time_t unix_time;
    static struct tm *time_info;
    unix_time = time(NULL);
    time_info = localtime(&unix_time);
    uint32_t year = time_info->tm_year + 1900;
    uint32_t month = time_info->tm_mon + 1;
    uint32_t day = time_info->tm_mday;
    printf("year:%d,month:%d,day:%d\n", year, month, day);
    if (lv_obj_get_parent(Calender_OBJ)!=NULL) {
        lv_obj_del(Calender_OBJ);
        printf("del\n");
        Calender_OBJ = NULL;  // 确保指针重置
    }
    Calender_OBJ = lv_obj_create(NULL);
    lv_obj_t *calendar_obj = lv_calendar_create(Calender_OBJ);
    static lv_style_t style_rect_back;
    lv_style_init(&style_rect_back);                  // 初始化样式
    lv_style_set_bg_color(&style_rect_back, lv_color_hex(0xf0e6e8));
    lv_obj_add_style(Calender_OBJ, &style_rect_back, 0);
    lv_obj_set_size(calendar_obj, (ver_res-rect_width)*0.7, hor_res*0.7);
    lv_obj_align(calendar_obj, LV_ALIGN_TOP_LEFT,(ver_res-rect_width-(ver_res-rect_width)*0.7)/2, (hor_res-hor_res*0.7)/2);
    // lv_obj_add_event_cb(calendar_obj, event_handler, LV_EVENT_ALL, NULL);
    lv_calendar_set_today_date(calendar_obj, year, month, day);
    lv_calendar_set_showed_date(calendar_obj, year, month);
    lv_calendar_header_dropdown_create(calendar_obj);
    lv_timer_t *claender_task_timer = lv_timer_create(clock_calender_task_callback, 1000, calendar_obj); // 创建定时任务，200ms刷新一次 

    static lv_style_t style_rect;
    lv_obj_t *rect;
    lv_color_t c = lv_color_hex(0xb4848c);       // 修改颜色值
    lv_style_init(&style_rect);                  // 初始化样式
    lv_style_set_bg_color(&style_rect, c);       // 设置颜色
    lv_style_set_width(&style_rect, rect_width); // 设置宽度
    lv_style_set_height(&style_rect, ver_res);   // 设置高度
    lv_style_set_radius(&style_rect, 0);         // 设置圆角
    // lv_style_set_opa(&style_rect,LV_OPA_COVER); //设置透明度
    lv_style_set_shadow_width(&style_rect, 25); // 设置阴影宽度
    lv_style_set_shadow_ofs_x(&style_rect, -3); // 设置水平偏移
    // lv_style_set_shadow_color(&style_rect,lv_palette_main(LV_PALETTE_NONE));//设置阴影颜色
    lv_style_set_border_width(&style_rect, 0); // 设置边框宽度
    rect = lv_obj_create(Calender_OBJ);
    lv_obj_align(rect, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_style(rect, &style_rect, 0);

    lv_obj_t *home_img_btn = lv_imgbtn_create(Calender_OBJ); // 创建在当前活动的屏幕
    // 设置正常状态下的图片
    lv_imgbtn_set_src(home_img_btn, LV_IMGBTN_STATE_RELEASED, &home, NULL, NULL);
    // lv_imgbtn_set_src(calendar_img_btn, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &calendar, NULL);
    lv_obj_align(home_img_btn, LV_ALIGN_TOP_RIGHT, -1 * rect_width / 2.0 + 64 / 2, ver_res / 2.0 - 64 / 2);
    lv_obj_set_size(home_img_btn, 64, 64);
    lv_obj_add_event_cb(home_img_btn, home_img_clicked_callback, LV_EVENT_CLICKED, NULL);

}
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
        HomePage();
        CalenderPage();
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

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
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
