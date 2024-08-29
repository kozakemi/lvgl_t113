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
/**
 * 初始化并显示一个矩形对象在屏幕上。
 */
typedef struct _lv_clock
{
    lv_obj_t *time_label; // 时间标签
    lv_obj_t *date_label; // 日期标签
    lv_obj_t *weekday_label; // 星期标签
}lv_clock_t;

static void clock_date_task_callback(lv_timer_t *timer)
{
    static const char *week_day[7] = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" };
    static time_t unix_time;
    static struct tm *time_info;
 
    unix_time = time(NULL);
    time_info = localtime(&unix_time);
 
    int year = time_info->tm_year+1900;
    int month = time_info->tm_mon + 1;
    int day = time_info->tm_mday;
    int weekday = time_info->tm_wday;
    int hour = time_info->tm_hour;
    int minutes = time_info->tm_min;
    int second = time_info->tm_sec;
 
    if (timer != NULL && timer->user_data != NULL)
    {
        lv_clock_t * clock = (lv_clock_t *)(timer->user_data);
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
            lv_obj_align(clock->weekday_label,LV_ALIGN_BOTTOM_MID,-110,-20);
         }
        }
    }
}
 
 
 
void my_display(void)
{
    lv_coord_t hor_res = lv_disp_get_hor_res(lv_disp_get_default());
    lv_coord_t ver_res = lv_disp_get_ver_res(lv_disp_get_default());
    // 声明图片资源
    LV_IMG_DECLARE(mikuimg); // 确保mikuimg.c和mikuimg.h文件已经生成
    /*miku IMG*/
    // 创建一个用于显示图片的图片对象
    lv_obj_t * img = lv_img_create(lv_scr_act()); // 创建在当前活动的屏幕
    // 设置图片对象的源为mikuimg
    lv_img_set_src(img, &mikuimg);
    lv_obj_align(img, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_coord_t rect_width = (lv_coord_t)((float)hor_res * 0.2f);//矩形宽度

    /*底部装饰1*/
    lv_obj_t* canvas = lv_canvas_create(lv_scr_act());
    static lv_color_t canvasBuf[(32 * 480) / 8 * 480];
    lv_canvas_set_buffer(canvas, canvasBuf, 480, 480, LV_IMG_CF_TRUE_COLOR_ALPHA);
    const lv_point_t polygonPoint[4] = { {0, ver_res-ver_res*0.1f}, {hor_res-rect_width, ver_res-ver_res*0.3f} , {hor_res-rect_width, ver_res} ,{0, ver_res} };
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_hex(0x567c8c);
    // rect_dsc.bg_opa=LV_OPA_10;
    lv_canvas_draw_polygon(canvas, polygonPoint, 4, &rect_dsc);


    /*底部装饰2*/
    lv_obj_t* canvas1 = lv_canvas_create(lv_scr_act());
    static lv_color_t canvasBuf1[(32 * 480) / 8 * 480];
    lv_canvas_set_buffer(canvas1, canvasBuf1, 480, 480, LV_IMG_CF_TRUE_COLOR_ALPHA);
    const lv_point_t polygonPoint1[4] = { {0, ver_res-ver_res*0.25f}, {hor_res-rect_width, ver_res-ver_res*0.45f} , {hor_res-rect_width, ver_res-ver_res*0.3f} ,{0, ver_res-ver_res*0.1f} };
    lv_draw_rect_dsc_t rect_dsc1;
    lv_draw_rect_dsc_init(&rect_dsc1);
    rect_dsc1.bg_color = lv_color_hex(0x000000);
    rect_dsc1.bg_opa=LV_OPA_80;
    lv_canvas_draw_polygon(canvas1, polygonPoint1, 4, &rect_dsc1);



    /*右侧栏*/
    static lv_style_t style_rect;
    lv_obj_t * rect;
    lv_color_t c = lv_color_hex(0xadc0c8); // 修改颜色值
    lv_style_init(&style_rect);//初始化样式
    lv_style_set_bg_color(&style_rect, c);//设置颜色
    lv_style_set_width(&style_rect, rect_width);//设置宽度
    lv_style_set_height(&style_rect, ver_res); //设置高度
    lv_style_set_radius(&style_rect, 0);//设置圆角
    // lv_style_set_opa(&style_rect,LV_OPA_COVER); //设置透明度
    lv_style_set_shadow_width(&style_rect,25);//设置阴影宽度
    lv_style_set_shadow_ofs_x(&style_rect,-3);//设置水平偏移
    // lv_style_set_shadow_color(&style_rect,lv_palette_main(LV_PALETTE_NONE));//设置阴影颜色
    lv_style_set_border_width(&style_rect,0);//设置边框宽度
    // lv_style_set_opa(&style_rect,LV_OPA_90);  
    // lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_ROW);
    // lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    rect = lv_obj_create(lv_scr_act());
    lv_obj_align(rect, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_style(rect, &style_rect, 0);
}


void test(void) {
    lv_coord_t hor_res = lv_disp_get_hor_res(lv_disp_get_default());
    lv_coord_t ver_res = lv_disp_get_ver_res(lv_disp_get_default());
    // 声明图片资源
    LV_IMG_DECLARE(mikuimg); // 确保mikuimg.c和mikuimg.h文件已经生成
    /*miku IMG*/
    // 创建一个用于显示图片的图片对象
    lv_obj_t * img = lv_img_create(lv_scr_act()); // 创建在当前活动的屏幕
    // 设置图片对象的源为mikuimg
    lv_img_set_src(img, &mikuimg);
    lv_obj_align(img, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_coord_t rect_width = (lv_coord_t)((float)hor_res * 0.2f);//矩形宽度

    /*底部装饰1*/
    lv_obj_t* canvas = lv_canvas_create(lv_scr_act());
    static lv_color_t canvasBuf[(32 * 480) / 8 * 480];
    lv_canvas_set_buffer(canvas, canvasBuf, 480, 480, LV_IMG_CF_TRUE_COLOR_ALPHA);
    const lv_point_t polygonPoint[4] = { {0, ver_res-ver_res*0.1f}, {hor_res-rect_width, ver_res-ver_res*0.3f} , {hor_res-rect_width, ver_res} ,{0, ver_res} };
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_hex(0x567c8c);
    // rect_dsc.bg_opa=LV_OPA_10;
    lv_canvas_draw_polygon(canvas, polygonPoint, 4, &rect_dsc);


    /*底部装饰2*/
    lv_obj_t* canvas1 = lv_canvas_create(lv_scr_act());
    static lv_color_t canvasBuf1[(32 * 480) / 8 * 480];
    lv_canvas_set_buffer(canvas1, canvasBuf1, 480, 480, LV_IMG_CF_TRUE_COLOR_ALPHA);
    const lv_point_t polygonPoint1[4] = { {0, ver_res-ver_res*0.25f}, {hor_res-rect_width, ver_res-ver_res*0.45f} , {hor_res-rect_width, ver_res-ver_res*0.3f} ,{0, ver_res-ver_res*0.1f} };
    lv_draw_rect_dsc_t rect_dsc1;
    lv_draw_rect_dsc_init(&rect_dsc1);
    rect_dsc1.bg_color = lv_color_hex(0x000000);
    rect_dsc1.bg_opa=LV_OPA_80;
    lv_canvas_draw_polygon(canvas1, polygonPoint1, 4, &rect_dsc1);



    /*右侧栏*/
    static lv_style_t style_rect;
    lv_obj_t * rect;
    lv_color_t c = lv_color_hex(0xadc0c8); // 修改颜色值
    lv_style_init(&style_rect);//初始化样式
    lv_style_set_bg_color(&style_rect, c);//设置颜色
    lv_style_set_width(&style_rect, rect_width);//设置宽度
    lv_style_set_height(&style_rect, ver_res); //设置高度
    lv_style_set_radius(&style_rect, 0);//设置圆角
    // lv_style_set_opa(&style_rect,LV_OPA_COVER); //设置透明度
    lv_style_set_shadow_width(&style_rect,25);//设置阴影宽度
    lv_style_set_shadow_ofs_x(&style_rect,-3);//设置水平偏移
    // lv_style_set_shadow_color(&style_rect,lv_palette_main(LV_PALETTE_NONE));//设置阴影颜色
    lv_style_set_border_width(&style_rect,0);//设置边框宽度
    // lv_style_set_opa(&style_rect,LV_OPA_90);  
    // lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_ROW);
    // lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    rect = lv_obj_create(lv_scr_act());
    lv_obj_align(rect, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_style(rect, &style_rect, 0);
    
        // 创建一个用于显示图片的图片对象
    lv_obj_t * shezhi_img = lv_img_create(lv_scr_act()); // 创建在当前活动的屏幕
    // 设置图片对象的源为mikuimg
    lv_img_set_src(shezhi_img, &shezhi);
    // lv_img_set_zoom(shezhi_img,64);
    lv_obj_align(shezhi_img, LV_ALIGN_TOP_RIGHT, -1*rect_width/2.0+64/2, 1*ver_res/5.0-64/2);

        // 创建一个用于显示图片的图片对象
    lv_obj_t * calendar_img = lv_img_create(lv_scr_act()); // 创建在当前活动的屏幕
    // 设置图片对象的源为mikuimg
    lv_img_set_src(calendar_img, &calendar);
    // lv_img_set_zoom(shezhi_img,64);
    lv_obj_align(calendar_img, LV_ALIGN_TOP_RIGHT, -1*rect_width/2.0+64/2, 2*ver_res/5.0-64/2);

            // 创建一个用于显示图片的图片对象
    lv_obj_t * comment_img = lv_img_create(lv_scr_act()); // 创建在当前活动的屏幕
    // 设置图片对象的源为mikuimg
    lv_img_set_src(comment_img, &comment);
    // lv_img_set_zoom(shezhi_img,64);
    lv_obj_align(comment_img, LV_ALIGN_TOP_RIGHT, -1*rect_width/2.0+64/2, 3*ver_res/5.0-64/2);



        // 创建一个用于显示图片的图片对象
    lv_obj_t * loading_img = lv_img_create(lv_scr_act()); // 创建在当前活动的屏幕
    // 设置图片对象的源为mikuimg
    lv_img_set_src(loading_img, &loading);
    // lv_img_set_zoom(shezhi_img,64);
    lv_obj_align(loading_img, LV_ALIGN_TOP_RIGHT, -1*rect_width/2.0+64/2, 4*ver_res/5.0-64/2);

//////////////////////////////////////////////////////////////////////////////////////////

    	/* Time font */
    static lv_style_t time_label_style; // 时间标签样式
    lv_style_init(&time_label_style); // 初始化样式
    lv_style_set_text_color(&time_label_style , lv_color_white()); // 设置标签样式文本颜色
    lv_style_set_text_font(&time_label_style, &lv_font_montserrat_32); // 设置字体风格
    lv_style_set_text_opa(&time_label_style, LV_OPA_COVER); // 设置字体透明度
    lv_style_set_bg_opa(&time_label_style, LV_OPA_0); // 设置样式背景透明度
    lv_style_set_text_color(&time_label_style,lv_color_hex(0xdba7af));
    
    // lv_style_set_transform_angle(&time_label_style, 300);
	/* Date font */
    static lv_style_t date_label_style; // 日期标签样式
    lv_style_init(&date_label_style);
    lv_style_set_text_opa(&date_label_style, LV_OPA_COVER);
    lv_style_set_bg_opa(&date_label_style, LV_OPA_0);
    lv_style_set_text_color(&date_label_style , lv_color_white());
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
    
    static lv_clock_t lv_clock = { 0 };
 
    lv_clock.time_label = lv_label_create(lv_scr_act()); // 基于time_obj对象创建时间显示标签对象 lv_clock.time_label
    lv_obj_add_style(lv_clock.time_label, &time_label_style, LV_STATE_DEFAULT); // 给对象 lv_clock.time_label添加样式

    lv_clock.date_label = lv_label_create(lv_scr_act()); // 基于date_obj对象创建lv_clock.date_label日期显示对象
    lv_obj_add_style(lv_clock.date_label, &date_label_style, LV_STATE_DEFAULT); // 给lv_clock.date_label对象添加样式

    /*Week display*/
    lv_clock.weekday_label = lv_label_create(lv_scr_act()); // 基于date_obj对象创建星期显示lv_clock.weekday_label对象
    lv_obj_add_style(lv_clock.weekday_label, &week_lable_style, LV_STATE_DEFAULT); // 给对象lv_clock.weekday_label添加样式

    lv_timer_t* task_timer = lv_timer_create(clock_date_task_callback, 200, (void *)&lv_clock); // 创建定时任务，200ms刷新一次


// ///////////////////////////////////////////////////
//   lv_draw_label_dsc_t label_dsc;
//   lv_draw_label_dsc_init(&label_dsc);
//   label_dsc.color = lv_palette_main(LV_PALETTE_YELLOW);
//   static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(480, 480)];
//   lv_obj_t * canvas23 = lv_canvas_create(lv_scr_act());
//   lv_canvas_set_buffer(canvas23, cbuf, 480, 480, LV_IMG_CF_TRUE_COLOR_ALPHA);
//   lv_obj_center(canvas23);
//   lv_canvas_draw_text(canvas23, 40, 20, 100, &label_dsc, "Some text on text canvas");
//   static lv_color_t cbuf_tmp[480 * 480];
//   memcpy(cbuf_tmp, cbuf, sizeof(cbuf_tmp));
//   lv_img_dsc_t img23;
//   img23.data = (void *)cbuf_tmp;
//   img23.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
//   img23.header.w = 480;
//   img23.header.h = 480;
//   // lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_NONE, 3), LV_OPA_COVER);
//   lv_canvas_transform(canvas23, &img23, 30, LV_IMG_ZOOM_NONE, 0, 0, 480 / 2, 480 / 2, true);


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
        test();
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
