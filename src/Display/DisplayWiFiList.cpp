#include "Display/DisplayWiFiList.h"
#include "Display/DisplayFullKeyboard.h"

SemaphoreHandle_t * DisplayWiFiList::s_pMutexLabelUpdate = nullptr;

TaskHandle_t DisplayWiFiList::s_WiFiScan;

lv_obj_t * DisplayWiFiList::s_pWindow = nullptr;
lv_obj_t * DisplayWiFiList::s_aLabels[MAX_WIFI_SSIDS_COUNT];

int16_t DisplayWiFiList::s_SSIDsCount = 0;

lv_obj_t * DisplayWiFiList::s_pExitButton   = nullptr;
lv_obj_t * DisplayWiFiList::s_pList         = nullptr;
lv_obj_t * DisplayWiFiList::s_pTempButton   = nullptr;

lv_obj_t * DisplayWiFiList::s_pLastSelectedButton = nullptr;

lv_style_t DisplayWiFiList::s_WiFiSSIDButtonStyle;
lv_style_t DisplayWiFiList::s_WiFiSSIDListStyle;
lv_style_t DisplayWiFiList::s_WiFiSSIDWindowTitle;

void DisplayWiFiList::Init(SemaphoreHandle_t * pMutex)
{
    s_pMutexLabelUpdate = pMutex;

    lv_style_init(              &s_WiFiSSIDWindowTitle );
    lv_style_set_text_color(    &s_WiFiSSIDWindowTitle, lv_color_black());
    lv_style_set_text_font(     &s_WiFiSSIDWindowTitle, &lv_font_montserrat_16);
    lv_style_set_border_color(  &s_WiFiSSIDWindowTitle, lv_color_black());
    lv_style_set_border_width(  &s_WiFiSSIDWindowTitle, 2);

    lv_style_init(              &s_WiFiSSIDButtonStyle );
    lv_style_set_bg_color(      &s_WiFiSSIDButtonStyle, lv_color_white());
    lv_style_set_border_color(  &s_WiFiSSIDButtonStyle, lv_color_black());
    lv_style_set_border_width(  &s_WiFiSSIDButtonStyle, 2);
    lv_style_set_text_color(    &s_WiFiSSIDButtonStyle, lv_color_black());
    lv_style_set_text_font(     &s_WiFiSSIDButtonStyle, &lv_font_montserrat_16);
    lv_style_set_radius(        &s_WiFiSSIDButtonStyle, 0);
    lv_style_set_align(         &s_WiFiSSIDButtonStyle, LV_ALIGN_LEFT_MID);

    lv_style_init(              &s_WiFiSSIDListStyle );
    lv_style_set_bg_color(      &s_WiFiSSIDListStyle, lv_color_hex(0x06818C));
    lv_style_set_border_color(  &s_WiFiSSIDListStyle, lv_color_black());
    lv_style_set_border_width(  &s_WiFiSSIDListStyle, 2);
    
    /*Create a window*/
    s_pWindow = lv_win_create(lv_screen_active());

    lv_obj_add_flag(                s_pWindow, LV_OBJ_FLAG_HIDDEN); 
    lv_win_add_title(               s_pWindow, "WIFi SSIDs list");
    lv_obj_set_align(               s_pWindow, LV_ALIGN_CENTER);
    lv_obj_set_pos(                 s_pWindow, 0, 0);
    lv_obj_set_size(                s_pWindow, lv_pct(90), lv_pct(90) );
    lv_obj_add_style(               s_pWindow, &s_WiFiSSIDWindowTitle, 0 );
    lv_obj_set_style_border_side(   s_pWindow, LV_BORDER_SIDE_INTERNAL, 0 );
    
    //window header
    lv_obj_t * pHeader = lv_win_get_header(s_pWindow);

    lv_obj_set_height(              pHeader, 28);
    lv_obj_set_style_border_color(  pHeader, lv_color_black(), 0);
    lv_obj_set_style_bg_color(      pHeader, lv_color_hex(0xB8B6B6), 0);
    lv_obj_set_style_border_width(  pHeader, 2, 0);
    lv_obj_set_style_pad_left(      pHeader, 0, 0);
    lv_obj_set_style_pad_right(     pHeader, 0, 0);
    lv_obj_set_style_pad_top(       pHeader, 0, 0);
    lv_obj_set_style_pad_bottom(    pHeader, 0, 0);

    //exit buton
    s_pExitButton = lv_win_add_button(s_pWindow, LV_SYMBOL_CLOSE, 28);

    lv_obj_set_width(               s_pExitButton, 28);
    lv_obj_set_height(              s_pExitButton, 28);
    lv_obj_add_event_cb(            s_pExitButton, exit_button_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_style(               s_pExitButton, &s_WiFiSSIDButtonStyle, 0);
    lv_obj_set_style_bg_color(      s_pExitButton, lv_color_hex(0xF03232), 0);

    //content of the window
    lv_obj_t * pContent = lv_win_get_content(s_pWindow);

    lv_obj_set_style_pad_left(  pContent, 0, 0);
    lv_obj_set_style_pad_right( pContent, 0, 0);
    lv_obj_set_style_pad_top(   pContent, 0, 0);
    lv_obj_set_style_pad_bottom(pContent, 0, 0);

    //*Create a list
    s_pList = lv_list_create(pContent);

    lv_obj_add_style(               s_pList, &s_WiFiSSIDListStyle, 0);
    lv_obj_set_size(                s_pList, lv_pct(100), lv_pct(100));
    lv_obj_set_style_radius(        s_pList, 0, 0);
    lv_obj_set_style_pad_top(       s_pList, 10, 0);
    lv_obj_set_style_pad_bottom(    s_pList, 10, 0);
    
    for(int i = 0; i < MAX_WIFI_SSIDS_COUNT; i++) 
    {
        s_pTempButton = lv_button_create(s_pList);

        lv_obj_set_width(               s_pTempButton, lv_pct(100));
        lv_obj_set_height(              s_pTempButton, lv_pct(10));
        lv_obj_set_style_pad_left(      s_pTempButton, 0, 0);
        lv_obj_set_style_pad_right(     s_pTempButton, 0, 0);
        lv_obj_set_style_pad_top(       s_pTempButton, 0, 0);
        lv_obj_set_style_pad_bottom(    s_pTempButton, 0, 0);
        lv_obj_add_event_cb(            s_pTempButton, list_event_handler, LV_EVENT_CLICKED, nullptr);
        lv_obj_add_style(               s_pTempButton, &s_WiFiSSIDButtonStyle, 0);

        lv_obj_add_flag(             s_pTempButton, LV_OBJ_FLAG_HIDDEN);

        s_aLabels[i] = lv_label_create(s_pTempButton);

        lv_label_set_text(s_aLabels[i], " ");
    }
}

void DisplayWiFiList::list_event_handler(lv_event_t * e)
{
    lv_obj_t * pObj = static_cast<lv_obj_t *>(lv_event_get_target(e));

    lv_obj_add_state(pObj, LV_STATE_CHECKED);
    if (s_pLastSelectedButton!= nullptr)    lv_obj_remove_state(s_pLastSelectedButton, LV_STATE_CHECKED); 

    DisplayFullKeyboard::SetVisible(true);

    s_pLastSelectedButton = pObj;
}

void DisplayWiFiList::exit_button_event_handler(lv_event_t * e)
{
    UpdateWiFiList(true);
}

void DisplayWiFiList::SetVisible(bool isVisible)
{
    if( xSemaphoreTake( *s_pMutexLabelUpdate, portMAX_DELAY ) == pdTRUE )
    {
        if(isVisible)
        {   
            lv_obj_remove_flag(s_pWindow, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(s_pWindow, LV_OBJ_FLAG_HIDDEN);
        }

        xSemaphoreGive(  *s_pMutexLabelUpdate);
    } 
}

void DisplayWiFiList::SetLabelTextAt(const char* pSSID, uint8_t index)
{
    if(index >= MAX_WIFI_SSIDS_COUNT) return;

    if( xSemaphoreTake( *s_pMutexLabelUpdate, portMAX_DELAY ) == pdTRUE )
    {
        lv_label_set_text(s_aLabels[index], pSSID);
        lv_obj_remove_flag( lv_obj_get_parent(s_aLabels[index]), LV_OBJ_FLAG_HIDDEN);

        xSemaphoreGive(  *s_pMutexLabelUpdate);
    }
}

void DisplayWiFiList::RemoveLabels()
{
    if( xSemaphoreTake( *s_pMutexLabelUpdate, portMAX_DELAY ) == pdTRUE )
    {
        if (s_pLastSelectedButton != nullptr) lv_obj_remove_state(s_pLastSelectedButton, LV_STATE_CHECKED); 

        for (size_t i = 0; i < s_SSIDsCount ; i++)
        {
            lv_obj_add_flag( lv_obj_get_parent(s_aLabels[i]), LV_OBJ_FLAG_HIDDEN);
        }

        xSemaphoreGive(  *s_pMutexLabelUpdate);
    }
}

void DisplayWiFiList::UpdateWiFiList(bool isExiting)
{
    xTaskCreate(taskWiFiScan,
                "WiFiScan",
                STACK_SIZE_WIFISCAN,
                &isExiting,
                10u,
                &s_WiFiScan);
}

void DisplayWiFiList::taskWiFiScan(void * isExiting)
{
    if( *static_cast<bool*>(isExiting) )//closing WiFilist window
    {
        RemoveLabels();
        SetVisible(false);
    }
    else// opening WiFilist window
    {
        SetVisible(true);

        s_SSIDsCount = WiFi.scanNetworks();

        if(s_SSIDsCount > MAX_WIFI_SSIDS_COUNT) s_SSIDsCount = MAX_WIFI_SSIDS_COUNT;

        for (size_t i = 0; i < s_SSIDsCount; i++)
        {
            SetLabelTextAt(WiFi.SSID(i).c_str(), i);
        }
        
    }

    Serial.printf("Task WiFiScan: %u Bytes free on stack\n", uxTaskGetStackHighWaterMark(s_WiFiScan));
    if( s_WiFiScan != NULL )
    {
        vTaskDelete( s_WiFiScan );
    }
    
}