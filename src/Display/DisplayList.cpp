#include "Display/DisplayList.h"


    lv_obj_t * DisplayList::list1           = nullptr;

    lv_obj_t * DisplayList::s_ButtonUp      = nullptr;
    lv_obj_t * DisplayList::s_ButtonDown    = nullptr;
    lv_obj_t * DisplayList::s_TempButton    = nullptr;
    lv_obj_t * DisplayList::currentButton   = nullptr;

DisplayList::DisplayList()
{
    lv_style_init(              &m_NavigationButtonStyle );
    lv_style_set_bg_color(      &m_NavigationButtonStyle, lv_color_hex(0x328BA8));
    lv_style_set_border_width(  &m_NavigationButtonStyle, 2);
    lv_style_set_border_color(  &m_NavigationButtonStyle, lv_color_black());
    lv_style_set_text_font(     &m_NavigationButtonStyle, &lv_font_montserrat_16);

    lv_style_init(              &m_WiFiSSIDButtonStyle );
    lv_style_set_bg_color(      &m_WiFiSSIDButtonStyle, lv_color_hex(0x328BA8));
    lv_style_set_border_width(  &m_WiFiSSIDButtonStyle, 2);
    lv_style_set_border_color(  &m_WiFiSSIDButtonStyle, lv_color_black());
    lv_style_set_text_color(    &m_WiFiSSIDButtonStyle, lv_color_black());
    lv_style_set_text_font(     &m_WiFiSSIDButtonStyle, &lv_font_montserrat_16);
    lv_style_set_radius(        &m_WiFiSSIDButtonStyle, 0);

    /*Create a list*/
    list1 = lv_list_create(lv_screen_active());
    lv_obj_set_size(list1, lv_pct(70), lv_pct(97));
    lv_obj_align(list1, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_set_style_radius(list1, 0, 0);

    /*Add buttons to the list*/
    
    for(int i = 0; i < MAX_WIFI_SSIDS_COUNT; i++) 
    {
        s_TempButton = lv_button_create(list1);
        lv_obj_set_width(s_TempButton, lv_pct(100));
        lv_obj_add_event_cb(s_TempButton, event_handler, LV_EVENT_CLICKED, nullptr);
        lv_obj_add_style(s_TempButton, &m_WiFiSSIDButtonStyle, 0);

        m_aLabels[i] = lv_label_create(s_TempButton);
        lv_label_set_text_fmt(m_aLabels[i], "Item %d", i);
    }

    /*Select the first button by default*/
    currentButton = lv_obj_get_child(list1, 0);
    lv_obj_add_state(currentButton, LV_STATE_CHECKED);

    s_ButtonUp = lv_button_create(lv_screen_active());
    m_pLabelUp = lv_label_create(s_ButtonUp);
    lv_label_set_text_fmt(m_pLabelUp, LV_SYMBOL_UP);
    lv_obj_align(m_pLabelUp, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(s_ButtonUp, event_handler_up, LV_EVENT_ALL, nullptr);
    lv_group_remove_obj(s_ButtonUp);
    lv_obj_add_style(s_ButtonUp, &m_NavigationButtonStyle, 0);
    lv_obj_align(s_ButtonUp, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_set_width(s_ButtonUp, lv_pct(20));
    lv_obj_set_height(s_ButtonUp, lv_pct(25));

    s_ButtonDown = lv_button_create(lv_screen_active());
    m_pLabelDown = lv_label_create(s_ButtonDown);
    lv_label_set_text_fmt(m_pLabelDown, LV_SYMBOL_DOWN);
    lv_obj_align(m_pLabelDown, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(s_ButtonDown, event_handler_dn, LV_EVENT_ALL, nullptr);
    lv_group_remove_obj(s_ButtonDown);
    lv_obj_add_style(s_ButtonDown, &m_NavigationButtonStyle, 0);
    lv_obj_align(s_ButtonDown, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
    lv_obj_set_width(s_ButtonDown, lv_pct(20));
    lv_obj_set_height(s_ButtonDown, lv_pct(25));
}

DisplayList::~DisplayList()
{
}

void DisplayList::event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = static_cast<lv_obj_t *>(lv_event_get_target(e));
    if(code == LV_EVENT_CLICKED) 
    {
        LV_LOG_USER("Clicked: %s", lv_list_get_button_text(list1, obj));

        if(currentButton == obj)    currentButton = NULL;
        else                        currentButton = obj;
        
        lv_obj_t * parent = lv_obj_get_parent(obj);

        for(uint32_t i = 0; i < lv_obj_get_child_count(parent); i++) 
        {
            lv_obj_t * child = lv_obj_get_child(parent, i);

            if(child == currentButton)  lv_obj_add_state(child, LV_STATE_CHECKED);
            else                        lv_obj_remove_state(child, LV_STATE_CHECKED);   
        }
    }
}

void DisplayList::event_handler_up(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) {
        if(currentButton == NULL) return;
        uint32_t index = lv_obj_get_index(currentButton);
        if(index <= 0) return;
        lv_obj_move_to_index(currentButton, index - 1);
        lv_obj_scroll_to_view(currentButton, LV_ANIM_ON);
    }
}

void DisplayList::event_handler_dn(lv_event_t * e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    if((code == LV_EVENT_CLICKED) || (code == LV_EVENT_LONG_PRESSED_REPEAT)) {
        if(currentButton == NULL) return;
        const uint32_t index = lv_obj_get_index(currentButton);

        lv_obj_move_to_index(currentButton, index + 1);
        lv_obj_scroll_to_view(currentButton, LV_ANIM_ON);
    }
}