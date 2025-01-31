#include "lvgl/lvgl.h"
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "home_page.h"

#include "source/img/home.h"

static lv_coord_t hor_res = 480;
static lv_coord_t ver_res = 480;
static lv_coord_t rect_width =(lv_coord_t)((float)480 * 0.2f);

lv_obj_t *MessagePage_OBJ;

static void home_img_clicked_callback(lv_event_t *e){
    LV_LOG_USER("Clicked");
    lv_scr_load_anim(HomePage_OBJ, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
}

void MessagePage(){

    MessagePage_OBJ=lv_obj_create(NULL);
    lv_obj_set_size(MessagePage_OBJ, LV_HOR_RES, LV_VER_RES);
    static lv_style_t style_rect_back;
    lv_style_init(&style_rect_back);                  // 初始化样式
    lv_style_set_bg_color(&style_rect_back, lv_color_hex(0xe3ecf4));
    lv_obj_add_style(MessagePage_OBJ, &style_rect_back, 0); // 设置背景颜色

    static lv_style_t Title_style;
    lv_obj_t *titlelabel = lv_label_create(MessagePage_OBJ);
    lv_label_set_text(titlelabel, "Sticky note");
    lv_obj_align(titlelabel, LV_ALIGN_CENTER, 0, 0);
    lv_style_set_text_font(&Title_style, &lv_font_montserrat_32);
    // lv_style_set_text_color(&Title_style, lv_color_white());
    lv_obj_add_style(titlelabel, &Title_style, 0);
    lv_obj_align(titlelabel, LV_ALIGN_TOP_LEFT, 20, 20);
    static lv_style_t style_shadow;
    lv_style_init(&style_shadow);
    lv_style_set_text_opa(&style_shadow, LV_OPA_30);
    lv_style_set_text_color(&style_shadow, lv_color_black());
    lv_obj_t * shadow_label = lv_label_create(MessagePage_OBJ);
    lv_obj_add_style(shadow_label, &style_shadow, 0);
    lv_label_set_text(shadow_label, lv_label_get_text(titlelabel));
    // lv_obj_align(shadow_label, LV_ALIGN_CENTER, 0, 0);
    lv_style_set_text_font(&style_shadow, &lv_font_montserrat_32);
    lv_obj_align_to(shadow_label, titlelabel, LV_ALIGN_TOP_LEFT, 2, 2);

    static lv_style_t content_style;
    lv_obj_t *contentlabel = lv_label_create(MessagePage_OBJ);
    lv_label_set_text(contentlabel, "My Name:Chen Shuo\n"
                                    " Away from my desk.\n"
                                    " Please call for urgent matters:\n"
                                    "  +86 19861557280,\n"
                                    " or contact via DingTalk/E-Mobile.");
    lv_obj_align(contentlabel, LV_ALIGN_CENTER, 0, 0);
    lv_style_set_text_font(&content_style, &lv_font_montserrat_20);
    // lv_style_set_text_color(&content_style, lv_color_white());
    lv_obj_add_style(contentlabel, &content_style, 0);
    lv_obj_align(contentlabel, LV_ALIGN_LEFT_MID, 25, 0);




    static lv_style_t style_rect;
    lv_obj_t *rect;
    lv_color_t c = lv_color_hex(0x71a2c6);       // 修改颜色值
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
    rect = lv_obj_create(MessagePage_OBJ);
    lv_obj_align(rect, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_style(rect, &style_rect, 0);


    lv_obj_t *home_img_btn = lv_imgbtn_create(MessagePage_OBJ); // 创建在当前活动的屏幕
    // 设置正常状态下的图片
    lv_imgbtn_set_src(home_img_btn, LV_IMGBTN_STATE_RELEASED, &home, NULL, NULL);
    // lv_imgbtn_set_src(calendar_img_btn, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &calendar, NULL);
    lv_obj_align(home_img_btn, LV_ALIGN_TOP_RIGHT, -1 * rect_width / 2.0 + 64 / 2, ver_res / 2.0 - 64 / 2);
    lv_obj_set_size(home_img_btn, 64, 64);
    lv_obj_add_event_cb(home_img_btn, home_img_clicked_callback, LV_EVENT_CLICKED, NULL);

}