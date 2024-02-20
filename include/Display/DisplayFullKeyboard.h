#ifndef DISPLAYFULLKEYBOARD_H
#define DISPLAYFULLKEYBOARD_H

#include "lvgl/lvgl.h"


class DisplayFullKeyboard
{
private:

    static lv_obj_t * s_Keyboard;
    static lv_obj_t * s_TextArea;

    DisplayFullKeyboard() = delete;
    ~DisplayFullKeyboard() = delete;

    static void ta_event_cb(lv_event_t * e);

public:

    static void Init();
    static void GetInputString(char * pDest);
    static void SetVisible(bool isVisible);

};

#endif