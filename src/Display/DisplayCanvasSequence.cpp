#include "Display/DisplayCanvasSequence.h"
#include "InlineDefinitions.h"

#include "Sprites/WiFi/WiFiSignal1.h"
#include "Sprites/WiFi/WiFiSignal2.h"
#include "Sprites/WiFi/WiFiSignal3.h"
#include "Sprites/WiFi/WiFiSignal4.h"

DisplayCanvasSequence::DisplayCanvasSequence(int16_t posX, int16_t posY)
{
    m_pCanvas = lv_canvas_create(lv_screen_active());
    lv_obj_set_pos(m_pCanvas, posX, posY);
    lv_canvas_set_buffer(m_pCanvas, static_cast<void*>(WiFiSignal4), 40, 40, LV_COLOR_FORMAT_RGB565);
}

DisplayCanvasSequence::~DisplayCanvasSequence()
{
}