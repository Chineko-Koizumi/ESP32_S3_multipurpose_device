#include "Display/DisplayWiFiList.h"

SemaphoreHandle_t * DisplayWiFiList::xMutexLabelUpdate = nullptr;

TaskHandle_t DisplayWiFiList::xWiFiScan;

lv_obj_t * DisplayWiFiList::m_pWindow = nullptr;
lv_obj_t * DisplayWiFiList::m_aLabels[MAX_WIFI_SSIDS_COUNT];

int16_t DisplayWiFiList::SSIDsCount = 0;

lv_obj_t * DisplayWiFiList::m_pExitButton   = nullptr;
lv_obj_t * DisplayWiFiList::m_pList         = nullptr;
lv_obj_t * DisplayWiFiList::m_pTempButton   = nullptr;

lv_style_t DisplayWiFiList::m_WiFiSSIDButtonStyle;
lv_style_t DisplayWiFiList::m_WiFiSSIDListStyle;
lv_style_t DisplayWiFiList::m_WiFiSSIDWindowTitle;

void DisplayWiFiList::init(SemaphoreHandle_t * pMutex)
{
    xMutexLabelUpdate = pMutex;

    lv_style_init(              &m_WiFiSSIDWindowTitle );
    lv_style_set_text_color(    &m_WiFiSSIDWindowTitle, lv_color_black());
    lv_style_set_text_font(     &m_WiFiSSIDWindowTitle, &lv_font_montserrat_16);
    lv_style_set_border_color(  &m_WiFiSSIDWindowTitle, lv_color_black());
    lv_style_set_border_width(  &m_WiFiSSIDWindowTitle, 2);

    lv_style_init(              &m_WiFiSSIDButtonStyle );
    lv_style_set_bg_color(      &m_WiFiSSIDButtonStyle, lv_color_white());
    lv_style_set_border_color(  &m_WiFiSSIDButtonStyle, lv_color_black());
    lv_style_set_border_width(  &m_WiFiSSIDButtonStyle, 2);
    lv_style_set_text_color(    &m_WiFiSSIDButtonStyle, lv_color_black());
    lv_style_set_text_font(     &m_WiFiSSIDButtonStyle, &lv_font_montserrat_16);
    lv_style_set_radius(        &m_WiFiSSIDButtonStyle, 0);
    lv_style_set_align(         &m_WiFiSSIDButtonStyle, LV_ALIGN_LEFT_MID);

    lv_style_init(              &m_WiFiSSIDListStyle );
    lv_style_set_bg_color(      &m_WiFiSSIDListStyle, lv_color_hex(0x06818C));
    lv_style_set_border_color(  &m_WiFiSSIDListStyle, lv_color_black());
    lv_style_set_border_width(  &m_WiFiSSIDListStyle, 2);
    
    /*Create a window*/
    m_pWindow = lv_win_create(lv_screen_active());

    lv_obj_add_flag(                m_pWindow, LV_OBJ_FLAG_HIDDEN); 
    lv_win_add_title(               m_pWindow, "WIFi SSIDs list");
    lv_obj_set_align(               m_pWindow, LV_ALIGN_CENTER);
    lv_obj_set_pos(                 m_pWindow, 0, 0);
    lv_obj_set_size(                m_pWindow, lv_pct(90), lv_pct(90) );
    lv_obj_add_style(               m_pWindow, &m_WiFiSSIDWindowTitle, 0 );
    lv_obj_set_style_border_side(   m_pWindow, LV_BORDER_SIDE_INTERNAL, 0 );
    
    //window header
    lv_obj_t * header = lv_win_get_header(m_pWindow);

    lv_obj_set_height(              header, 28);
    lv_obj_set_style_border_color(  header, lv_color_black(), 0);
    lv_obj_set_style_bg_color(      header, lv_color_hex(0xB8B6B6), 0);
    lv_obj_set_style_border_width(  header, 2, 0);
    lv_obj_set_style_pad_left(      header, 0, 0);
    lv_obj_set_style_pad_right(     header, 0, 0);
    lv_obj_set_style_pad_top(       header, 0, 0);
    lv_obj_set_style_pad_bottom(    header, 0, 0);

    //exit buton
    m_pExitButton = lv_win_add_button(m_pWindow, LV_SYMBOL_CLOSE, 28);

    lv_obj_set_width(               m_pExitButton, 28);
    lv_obj_set_height(              m_pExitButton, 28);
    lv_obj_add_event_cb(            m_pExitButton, exit_button_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_style(               m_pExitButton, &m_WiFiSSIDButtonStyle, 0);
    lv_obj_set_style_bg_color(      m_pExitButton, lv_color_hex(0xF03232), 0);

    //content of the window
    lv_obj_t * content = lv_win_get_content(m_pWindow);

    lv_obj_set_style_pad_left(  content, 0, 0);
    lv_obj_set_style_pad_right( content, 0, 0);
    lv_obj_set_style_pad_top(   content, 0, 0);
    lv_obj_set_style_pad_bottom(content, 0, 0);

    //*Create a list
    m_pList = lv_list_create(content);

    lv_obj_add_style(               m_pList, &m_WiFiSSIDListStyle, 0);
    lv_obj_set_size(                m_pList, lv_pct(100), lv_pct(100));
    lv_obj_set_style_radius(        m_pList, 0, 0);
    lv_obj_set_style_pad_top(       m_pList, 10, 0);
    lv_obj_set_style_pad_bottom(    m_pList, 10, 0);
    
    for(int i = 0; i < MAX_WIFI_SSIDS_COUNT; i++) 
    {
        m_pTempButton = lv_button_create(m_pList);

        lv_obj_set_width(               m_pTempButton, lv_pct(100));
        lv_obj_set_height(              m_pTempButton, lv_pct(10));
        lv_obj_set_style_pad_left(      m_pTempButton, 0, 0);
        lv_obj_set_style_pad_right(     m_pTempButton, 0, 0);
        lv_obj_set_style_pad_top(       m_pTempButton, 0, 0);
        lv_obj_set_style_pad_bottom(    m_pTempButton, 0, 0);
        lv_obj_add_event_cb(            m_pTempButton, list_event_handler, LV_EVENT_CLICKED, nullptr);
        lv_obj_add_style(               m_pTempButton, &m_WiFiSSIDButtonStyle, 0);
        lv_obj_add_flag(             m_pTempButton, LV_OBJ_FLAG_HIDDEN);

        m_aLabels[i] = lv_label_create(m_pTempButton);

        lv_label_set_text(m_aLabels[i], " ");
    }
}

void DisplayWiFiList::list_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    static lv_obj_t * last_obj  = nullptr;
    lv_obj_t * obj              = static_cast<lv_obj_t *>(lv_event_get_target(e));

    lv_obj_add_state(obj, LV_STATE_CHECKED);
    if (last_obj != nullptr)    lv_obj_remove_state(last_obj, LV_STATE_CHECKED); 

    last_obj = obj;
}

void DisplayWiFiList::exit_button_event_handler(lv_event_t * e)
{
    UpdateWiFiList(true);
}

void DisplayWiFiList::SetVisible(bool isVisible)
{
    if( xSemaphoreTake( *xMutexLabelUpdate, portMAX_DELAY ) == pdTRUE )
    {
        if(isVisible)
        {   
            lv_obj_remove_flag(m_pWindow, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(m_pWindow, LV_OBJ_FLAG_HIDDEN);
        }

        xSemaphoreGive(  *xMutexLabelUpdate);
    } 
}

void DisplayWiFiList::SetLabelTextAt(const char* pSSID, uint8_t index)
{
    if(index >= MAX_WIFI_SSIDS_COUNT) return;

    if( xSemaphoreTake( *xMutexLabelUpdate, portMAX_DELAY ) == pdTRUE )
    {
        lv_label_set_text(m_aLabels[index], pSSID);
        lv_obj_remove_flag( lv_obj_get_parent(m_aLabels[index]), LV_OBJ_FLAG_HIDDEN);

        xSemaphoreGive(  *xMutexLabelUpdate);
    }
}

void DisplayWiFiList::RemoveLabels()
{
    if( xSemaphoreTake( *xMutexLabelUpdate, portMAX_DELAY ) == pdTRUE )
    {
        for (size_t i = 0; i < SSIDsCount ; i++)
        {
            lv_obj_add_flag( lv_obj_get_parent(m_aLabels[i]), LV_OBJ_FLAG_HIDDEN);
        }

        xSemaphoreGive(  *xMutexLabelUpdate);
    }
}

void DisplayWiFiList::UpdateWiFiList(bool isExiting)
{
    xTaskCreate(taskWiFiScan,
                "WiFiScan",
                STACK_SIZE_WIFISCAN,
                &isExiting,
                10u,
                &xWiFiScan);
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

        SSIDsCount = WiFi.scanNetworks();

        if(SSIDsCount > MAX_WIFI_SSIDS_COUNT) SSIDsCount = MAX_WIFI_SSIDS_COUNT;

        for (size_t i = 0; i < SSIDsCount; i++)
        {
            SetLabelTextAt(WiFi.SSID(i).c_str(), i);
        }
        
    }

    Serial.printf("Task WiFiScan: %u Bytes free on stack\n", uxTaskGetStackHighWaterMark(xWiFiScan));
    if( xWiFiScan != NULL )
    {
        vTaskDelete( xWiFiScan );
    }
    
}