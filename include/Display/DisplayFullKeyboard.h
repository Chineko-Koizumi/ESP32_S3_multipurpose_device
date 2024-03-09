#ifndef DISPLAYFULLKEYBOARD_H
#define DISPLAYFULLKEYBOARD_H

#include "lvgl/lvgl.h"
#include "InlineDefinitions.h"

class DisplayFullKeyboard
{
private:

    static const uint16_t STACK_SIZE_SETVISIBLE = 4000U;

    static TaskHandle_t s_TaskHandleSetVisible;

    static lv_obj_t * s_pGreyBackground;
    static lv_obj_t * s_pKeyboard;
    static lv_obj_t * s_pTextArea;

    static SemaphoreHandle_t *s_pMutexUpdate;

    DisplayFullKeyboard() = delete;
    ~DisplayFullKeyboard() = delete;

    static void ta_event_cb(lv_event_t * e);
    static void kb_event_cb(lv_event_t * e);

    static void taskSetVisible(void * isVisible);
    static void SetVisible(bool isVisible);

public:
    static void Init(SemaphoreHandle_t * pMutex);
    static void GetInputString(char * pDest);
    static void UpdateSetVisible(bool isVisible);

};

#endif