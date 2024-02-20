#include "Display/DisplayImageSequence.h"
#include "InlineDefinitions.h"

#include "Sprites/WiFi/noWiFi.h"
#include "Sprites/WiFi/WiFiSignal1.h"
#include "Sprites/WiFi/WiFiSignal2.h"
#include "Sprites/WiFi/WiFiSignal3.h"
#include "Sprites/WiFi/WiFiSignal4.h"

DisplayImageSequenceBase::~DisplayImageSequenceBase(){}

DisplayImageSequenceWiFiSignal::DisplayImageSequenceWiFiSignal(SemaphoreHandle_t *pRenderingMuxtex, int16_t posX, int16_t posY)
{
    DisplayImageSequenceBase::m_pRenderingMutex = pRenderingMuxtex;

    m_pImage        = lv_image_create(lv_screen_active());
    m_SequenceCount = 4U;

    m_aBuffersArray = new lv_image_dsc_t*[m_SequenceCount];

    lv_obj_align(       m_pImage, LV_ALIGN_TOP_LEFT, posX, posY);
    lv_obj_add_flag(    m_pImage, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(m_pImage, click_event_handler, LV_EVENT_CLICKED, nullptr);

    DisplayWiFiList::Init(m_pRenderingMutex);

    AddBuffer( &noWiFi );
    AddBuffer( &WiFiSignal1 );
    AddBuffer( &WiFiSignal2 );
    AddBuffer( &WiFiSignal3 );
    AddBuffer( &WiFiSignal4 );
}

DisplayImageSequenceWiFiSignal::~DisplayImageSequenceWiFiSignal()
{
    delete[] m_aBuffersArray;
}

void DisplayImageSequenceWiFiSignal::AddBuffer(lv_image_dsc_t* pBuffer)
{
    if(m_AddedBuffersIndex >= m_SequenceCount) return;

    m_aBuffersArray[m_AddedBuffersIndex] = pBuffer;
    ++m_AddedBuffersIndex;
}

void DisplayImageSequenceWiFiSignal::SetFrame(uint8_t frameNumber)
{
    if(frameNumber >= m_SequenceCount) return;

    if( xSemaphoreTake( *m_pRenderingMutex, portMAX_DELAY ) == pdTRUE )
    {
        lv_image_set_src( m_pImage, m_aBuffersArray[frameNumber] );

        xSemaphoreGive(  *m_pRenderingMutex);
    } 
}

void DisplayImageSequenceWiFiSignal::SetSignalStrength(int8_t RSSI)
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

void DisplayImageSequenceWiFiSignal::click_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = static_cast<lv_obj_t *>(lv_event_get_target(e));
    DisplayWiFiList::UpdateWiFiList(false); //isExiting to false, this is entering phase
}