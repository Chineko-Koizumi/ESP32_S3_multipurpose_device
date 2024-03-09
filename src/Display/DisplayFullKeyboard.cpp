#include "Display/DisplayFullKeyboard.h"

lv_obj_t * DisplayFullKeyboard::s_pKeyboard = nullptr;
lv_obj_t * DisplayFullKeyboard::s_pTextArea = nullptr;
lv_obj_t * DisplayFullKeyboard::s_pGreyBackground = nullptr;

SemaphoreHandle_t * DisplayFullKeyboard::s_pMutexUpdate = nullptr;

TaskHandle_t DisplayFullKeyboard::s_TaskHandleSetVisible = NULL;

void DisplayFullKeyboard::Init(SemaphoreHandle_t * pMutex)
{
    s_pMutexUpdate = pMutex;

    s_pGreyBackground = lv_obj_create(lv_scr_act());

    lv_obj_set_size(            s_pGreyBackground, MAX_IMAGE_WIDTH, MAX_IMAGE_HIGHT);
    lv_obj_align(               s_pGreyBackground, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(  s_pGreyBackground, lv_color_hex(0xF03232), 0);
    lv_obj_set_style_bg_opa(    s_pGreyBackground, 200, 0);

    lv_obj_add_flag( s_pGreyBackground, LV_OBJ_FLAG_HIDDEN);

    /*Create a keyboard to use it with an of the text areas*/
    s_pKeyboard = lv_keyboard_create(s_pGreyBackground);

    lv_obj_add_event_cb(s_pKeyboard, kb_event_cb, LV_EVENT_ALL, nullptr);

    /*Create a text area. The keyboard will write here*/
    s_pTextArea = lv_textarea_create(s_pGreyBackground);

    lv_obj_align(                       s_pTextArea, LV_ALIGN_CENTER, 0, -25);
    lv_obj_add_event_cb(                s_pTextArea, ta_event_cb, LV_EVENT_ALL, s_pKeyboard);
    lv_textarea_set_placeholder_text(   s_pTextArea, "PASSWORD");
    lv_obj_set_size(                    s_pTextArea, lv_pct(90), lv_pct(8));

    lv_obj_set_style_text_align(    s_pTextArea, LV_ALIGN_OUT_LEFT_MID, 0);
    lv_obj_set_style_radius(        s_pTextArea, 0, 0);
    lv_obj_set_style_border_color(  s_pTextArea, lv_color_black(), 0);
    lv_obj_set_style_border_width(  s_pTextArea, 2, 0);
    lv_obj_set_style_pad_top(       s_pTextArea, 0, 0);
    lv_obj_set_style_pad_bottom(    s_pTextArea, 0, 0);

    lv_keyboard_set_textarea(s_pKeyboard, s_pTextArea);
}

void DisplayFullKeyboard::kb_event_cb(lv_event_t * e)
{
   lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CANCEL) 
    {
        UpdateSetVisible(false);
        return;
    }
}

void DisplayFullKeyboard::ta_event_cb(lv_event_t * e)
{

}

void DisplayFullKeyboard::GetInputString(char * pDest)
{
    const char * pSrc = lv_textarea_get_text(s_pTextArea);
    memcpy(pDest, pSrc, strlen(pSrc));
}

void DisplayFullKeyboard::SetVisible(bool isVisible)
{
    if( xSemaphoreTake( *s_pMutexUpdate, portMAX_DELAY ) == pdTRUE )
    {
        if(isVisible)
        {
            lv_textarea_set_text(s_pTextArea, "");
            lv_obj_remove_flag( s_pGreyBackground,  LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag( s_pGreyBackground, LV_OBJ_FLAG_HIDDEN);
        }

        xSemaphoreGive(  *s_pMutexUpdate);
    } 
}

void DisplayFullKeyboard::UpdateSetVisible(bool isVisible)
{
    if(s_TaskHandleSetVisible != NULL) return;
    xTaskCreate(taskSetVisible,
                "SetVisible",
                STACK_SIZE_SETVISIBLE,
                &isVisible,
                10u,
                &s_TaskHandleSetVisible);
}

void DisplayFullKeyboard::taskSetVisible(void * isVisible)
{
    SetVisible(*static_cast<bool*>(isVisible));

    s_TaskHandleSetVisible = NULL;
    vTaskDelete( NULL );  
}
