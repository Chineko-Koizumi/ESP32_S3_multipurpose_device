#ifndef DISPLAYWIFILIST_H
#define DISPLAYWIFILIST_H

#include <WiFi.h>

#include "lvgl/lvgl.h"

class DisplayWiFiList
{
private:
    static const uint8_t MAX_WIFI_SSIDS_COUNT = 15U; 
    static const uint16_t STACK_SIZE_WIFISCAN = 4000U;

    static int16_t s_SSIDsCount; 

    static SemaphoreHandle_t *s_pMutexLabelUpdate;

    static lv_obj_t *s_aLabels[MAX_WIFI_SSIDS_COUNT];

    static TaskHandle_t s_WiFiScan;

    static lv_obj_t * s_pWindow;
    static lv_obj_t * s_pExitButton;
    static lv_obj_t * s_pList;
    static lv_obj_t * s_pTempButton;

    static lv_obj_t * s_pLastSelectedButton;
    
    static lv_style_t s_WiFiSSIDButtonStyle;
    static lv_style_t s_WiFiSSIDListStyle;
    static lv_style_t s_WiFiSSIDWindowTitle;

    static void list_event_handler(lv_event_t * e);
    static void exit_button_event_handler(lv_event_t * e);

    static void taskWiFiScan(void * pvParameters);

    DisplayWiFiList()   = delete;
    ~DisplayWiFiList()  = delete;

public:

    static void Init(SemaphoreHandle_t * pMutex);
    static void SetVisible(bool isVisible);
    static void UpdateWiFiList(bool isExiting);
    static void SetLabelTextAt(const char* pSSID, uint8_t index);
    static void RemoveLabels();
};

#endif
