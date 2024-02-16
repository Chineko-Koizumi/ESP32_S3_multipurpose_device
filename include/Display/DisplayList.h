#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include "lvgl/lvgl.h"


class DisplayList
{
private:
    static const uint8_t MAX_WIFI_SSIDS_COUNT = 15U;

    lv_obj_t *m_aLabels[MAX_WIFI_SSIDS_COUNT];

    static lv_obj_t * list1;

    static lv_obj_t * s_ButtonUp;
    static lv_obj_t * s_ButtonDown;
    static lv_obj_t * s_TempButton;
    static lv_obj_t * currentButton;

    lv_obj_t * m_pLabelUp      {nullptr};
    lv_obj_t * m_pLabelDown    {nullptr};

    lv_style_t m_NavigationButtonStyle;
    lv_style_t m_WiFiSSIDButtonStyle;

public:
    DisplayList();
    ~DisplayList();

    static void event_handler(lv_event_t * e);
    static void event_handler_top(lv_event_t * e);
    static void event_handler_up(lv_event_t * e);
    static void event_handler_center(lv_event_t * e);
    static void event_handler_dn(lv_event_t * e);
    static void event_handler_bottom(lv_event_t * e);
    static void event_handler_swap(lv_event_t * e);

};

#endif
