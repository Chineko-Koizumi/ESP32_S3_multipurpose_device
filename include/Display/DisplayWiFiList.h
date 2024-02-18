#ifndef DISPLAYWIFILIST_H
#define DISPLAYWIFILIST_H

#include <WiFi.h>

#include "lvgl/lvgl.h"

class DisplayWiFiList
{
private:
    static const uint8_t MAX_WIFI_SSIDS_COUNT = 15U; 
    static const uint16_t STACK_SIZE_WIFISCAN = 4000U;

    static int16_t SSIDsCount; 

    static SemaphoreHandle_t *xMutexLabelUpdate;

    static lv_obj_t *m_aLabels[MAX_WIFI_SSIDS_COUNT];

    static TaskHandle_t xWiFiScan;

    static lv_obj_t * m_pWindow;
    static lv_obj_t * m_pExitButton;
    static lv_obj_t * m_pList;
    static lv_obj_t * m_pTempButton;
    
    static lv_style_t m_WiFiSSIDButtonStyle;
    static lv_style_t m_WiFiSSIDListStyle;
    static lv_style_t m_WiFiSSIDWindowTitle;

    static void list_event_handler(lv_event_t * e);
    static void exit_button_event_handler(lv_event_t * e);

    static void taskWiFiScan(void * pvParameters);

public:
    DisplayWiFiList()   = delete;
    ~DisplayWiFiList()  = delete;

    static void init(SemaphoreHandle_t * pMutex);
    static void SetVisible(bool isVisible);
    static void UpdateWiFiList(bool isExiting);
    static void SetLabelTextAt(const char* pSSID, uint8_t index);
    static void RemoveLabels();
};

#endif
