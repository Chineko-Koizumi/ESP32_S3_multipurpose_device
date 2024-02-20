#include "Display/DisplayFullKeyboard.h"

lv_obj_t * DisplayFullKeyboard::s_Keyboard = nullptr;
lv_obj_t * DisplayFullKeyboard::s_TextArea = nullptr;

void DisplayFullKeyboard::Init()
{
    /*Create a keyboard to use it with an of the text areas*/
    s_Keyboard = lv_keyboard_create(lv_screen_active());

    lv_obj_add_flag( s_Keyboard, LV_OBJ_FLAG_HIDDEN);

    /*Create a text area. The keyboard will write here*/
    
    s_TextArea = lv_textarea_create(lv_screen_active());

    lv_obj_add_flag( s_TextArea, LV_OBJ_FLAG_HIDDEN);

    lv_obj_align(                       s_TextArea, LV_ALIGN_CENTER, 0, -25);
    lv_obj_add_event_cb(                s_TextArea, ta_event_cb, LV_EVENT_ALL, s_Keyboard);
    lv_textarea_set_placeholder_text(   s_TextArea, "PASSWORD");
    lv_obj_set_size(                    s_TextArea, lv_pct(90), lv_pct(8));

    lv_obj_set_style_text_align(    s_TextArea, LV_ALIGN_OUT_LEFT_MID, 0);
    lv_obj_set_style_radius(        s_TextArea, 0, 0);
    lv_obj_set_style_border_color(  s_TextArea, lv_color_black(), 0);
    lv_obj_set_style_border_width(  s_TextArea, 2, 0);
    lv_obj_set_style_pad_top(       s_TextArea, 0, 0);
    lv_obj_set_style_pad_bottom(    s_TextArea, 0, 0);

    lv_keyboard_set_textarea(s_Keyboard, s_TextArea);
}

void DisplayFullKeyboard::ta_event_cb(lv_event_t * e)
{ }

void DisplayFullKeyboard::GetInputString(char * pDest)
{
    const char * pSrc = lv_textarea_get_text(s_TextArea);
    memcpy(pDest, pSrc, strlen(pSrc));
}

void DisplayFullKeyboard::SetVisible(bool isVisible)
{
    if(isVisible)
    {
        lv_obj_remove_flag( s_Keyboard, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag( s_TextArea, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag( s_Keyboard, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag( s_TextArea, LV_OBJ_FLAG_HIDDEN);
    }
}
