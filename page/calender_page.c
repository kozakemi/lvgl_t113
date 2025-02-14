/**
 * @file calender_page.c
 * @brief 实现日历页面的功能，包括显示当前日期、定时更新日期以及返回主页按钮。
 * @author Kozakemi (kemikoza@gmail.com)
 * @date 2025-02-14
 * @copyright Copyright (c) 2025  Kozakemi
 * 
 * @par 功能描述
 * -# 创建并显示日历页面。
 * -# 定时更新日历显示的日期。
 * -# 提供返回主页按钮，点击后返回主页。
 * 
 * @par 用法描述
 * -# 调用 `CalenderPage()` 函数初始化并显示日历页面。
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2025-02-14 <td>1.0     <td>Kozakemi  <td>初始版本，实现日历页面的基本功能。
 * </table>
 */
#include "lvgl/lvgl.h"
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "calender_page.h"
#include "home_page.h"
#include "source/img/home.h"


lv_obj_t *Calender_OBJ;
static lv_coord_t hor_res = 480;
static lv_coord_t ver_res = 480;
static lv_coord_t rect_width =(lv_coord_t)((float)480 * 0.2f);
/**
 * @brief 主页按键触发回调
 * @note  跳转回主页面
 * @param {lv_event_t *} e 
 */
static void home_img_clicked_callback(lv_event_t *e){
    LV_LOG_USER("Clicked");
    // lv_scr_load(HomePage_OBJ);
    lv_scr_load_anim(HomePage_OBJ, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
}

/**
 * @brief 时钟定时触发回调
 * @param {lv_timer_t *} timer 
 */
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
/**
 * @brief 日历页面
 * 
 */
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