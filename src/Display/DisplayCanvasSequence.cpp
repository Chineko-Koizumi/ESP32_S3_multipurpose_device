#include "Display/DisplayCanvasSequence.h"
#include "InlineDefinitions.h"

#include "Sprites/WiFi/WiFiSignal1.h"
#include "Sprites/WiFi/WiFiSignal2.h"
#include "Sprites/WiFi/WiFiSignal3.h"
#include "Sprites/WiFi/WiFiSignal4.h"

DisplayCanvasSequenceBase::~DisplayCanvasSequenceBase(){}

DisplayCanvasSequenceWiFiSignal::DisplayCanvasSequenceWiFiSignal(int16_t posX, int16_t posY)
{
    m_pCanvas       = lv_canvas_create(lv_screen_active());
    m_SequenceCount = 4U;

    m_aBuffersArray = new uint8_t*[m_SequenceCount];

    lv_obj_set_pos(m_pCanvas, posX, posY);

    AddBuffer(const_cast<uint8_t*>(WiFiSignal1_map));
    AddBuffer(const_cast<uint8_t*>(WiFiSignal2_map));
    AddBuffer(const_cast<uint8_t*>(WiFiSignal3_map));
    AddBuffer(const_cast<uint8_t*>(WiFiSignal4_map));
}

DisplayCanvasSequenceWiFiSignal::~DisplayCanvasSequenceWiFiSignal()
{
    delete[] m_aBuffersArray;
}

void DisplayCanvasSequenceWiFiSignal::AddBuffer(uint8_t* pBuffer)
{
    if(m_AddedBuffersIndex <= m_SequenceCount) return;

    m_aBuffersArray[m_AddedBuffersIndex] = pBuffer;
    ++m_AddedBuffersIndex;
}

void DisplayCanvasSequenceWiFiSignal::SetFrame(uint8_t frameNumber)
{
    if(frameNumber >= m_SequenceCount) return;

    lv_canvas_set_buffer(m_pCanvas, reinterpret_cast<void*>(m_aBuffersArray[frameNumber]), 40, 40, LV_COLOR_FORMAT_ARGB8888 );
}

void DisplayCanvasSequenceWiFiSignal::SetSignalStrength(int8_t RSSI)
{
    if((WIFI_SIGNAL_VERY_GOOD <= RSSI))
    {
        if(m_LastRSSI != WIFI_SIGNAL_VERY_GOOD)
        {
            SetFrame(3);
            m_LastRSSI = WIFI_SIGNAL_VERY_GOOD;
        }
    }
    else if(WIFI_SIGNAL_GOOD <= RSSI)
    {
        if(m_LastRSSI != WIFI_SIGNAL_GOOD)
        {
            SetFrame(2);
            m_LastRSSI = WIFI_SIGNAL_GOOD;
        }
    }
    else if(WIFI_SIGNAL_LOW <= RSSI)
    {
        if(m_LastRSSI != WIFI_SIGNAL_LOW)
        {
            SetFrame(1);
            m_LastRSSI = WIFI_SIGNAL_LOW;
        }
    }
    else
    {
        if(m_LastRSSI != WIFI_SIGNAL_VERY_LOW)
        {
            SetFrame(0);
            m_LastRSSI = WIFI_SIGNAL_VERY_LOW;
        }
    }
}