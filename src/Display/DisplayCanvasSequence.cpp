#include "Display/DisplayCanvasSequence.h"
#include "InlineDefinitions.h"

#include "Sprites/WiFi/noWiFi.h"
#include "Sprites/WiFi/WiFiSignal1.h"
#include "Sprites/WiFi/WiFiSignal2.h"
#include "Sprites/WiFi/WiFiSignal3.h"
#include "Sprites/WiFi/WiFiSignal4.h"

DisplayCanvasSequenceBase::~DisplayCanvasSequenceBase(){}

DisplayCanvasSequenceWiFiSignal::DisplayCanvasSequenceWiFiSignal(int16_t posX, int16_t posY)
{
    m_pImage        = lv_image_create(lv_screen_active());
    m_SequenceCount = 4U;

    m_aBuffersArray = new lv_image_dsc_t*[m_SequenceCount];

    lv_obj_align(m_pImage, LV_ALIGN_TOP_LEFT, posX, posY);

    AddBuffer( &noWiFi );
    AddBuffer( &WiFiSignal1 );
    AddBuffer( &WiFiSignal2 );
    AddBuffer( &WiFiSignal3 );
    AddBuffer( &WiFiSignal4 );
}

DisplayCanvasSequenceWiFiSignal::~DisplayCanvasSequenceWiFiSignal()
{
    delete[] m_aBuffersArray;
}

void DisplayCanvasSequenceWiFiSignal::AddBuffer(lv_image_dsc_t* pBuffer)
{
    if(m_AddedBuffersIndex >= m_SequenceCount) return;

    m_aBuffersArray[m_AddedBuffersIndex] = pBuffer;
    ++m_AddedBuffersIndex;
}

void DisplayCanvasSequenceWiFiSignal::SetFrame(uint8_t frameNumber)
{
    if(frameNumber >= m_SequenceCount) return;

    lv_image_set_src( m_pImage, m_aBuffersArray[frameNumber] );
}

void DisplayCanvasSequenceWiFiSignal::SetSignalStrength(int8_t RSSI)
{
    if(RSSI == 0)SetFrame(NO_SIGNAL);

    if((WIFI_SIGNAL_VERY_GOOD <= RSSI))
    {
        if(m_LastRSSI != WIFI_SIGNAL_VERY_GOOD)
        {
            SetFrame(SIGNAL_VERY_GOOD);
            m_LastRSSI = WIFI_SIGNAL_VERY_GOOD;
        }
    }
    else if(WIFI_SIGNAL_GOOD <= RSSI)
    {
        if(m_LastRSSI != WIFI_SIGNAL_GOOD)
        {
            SetFrame(SIGNAL_GOOD);
            m_LastRSSI = WIFI_SIGNAL_GOOD;
        }
    }
    else if(WIFI_SIGNAL_LOW <= RSSI)
    {
        if(m_LastRSSI != WIFI_SIGNAL_LOW)
        {
            SetFrame(SIGNAL_MEDIUM);
            m_LastRSSI = WIFI_SIGNAL_LOW;
        }
    }
    else
    {
        if(m_LastRSSI != WIFI_SIGNAL_VERY_LOW)
        {
            SetFrame(SIGNAL_LOW);
            m_LastRSSI = WIFI_SIGNAL_VERY_LOW;
        }
    }
}