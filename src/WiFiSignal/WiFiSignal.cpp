#include <WiFi.h>

#include "WiFiSignal/WiFiSignal.h"

#include "Sprites/WiFi/WiFiSignal1.h"
#include "Sprites/WiFi/WiFiSignal2.h"
#include "Sprites/WiFi/WiFiSignal3.h"
#include "Sprites/WiFi/WiFiSignal4.h"


WiFiSignal::WiFiSignal(TFT_eSPI *pScreen, const Coordinates& spritePossition):
m_CurrentSprite(pScreen),
m_SpritePossition(spritePossition),
m_LastRSSI(WIFI_SIGNAL_LOW)
{

}

WiFiSignal::~WiFiSignal()
{

}

void WiFiSignal::CreateSprite()
{
    m_CurrentSprite.createSprite(WiFiSignal1.width, WiFiSignal1.height);
    setBackground(WiFiSignal4.pixel_data);
}

void WiFiSignal::setBackground(uint16_t *pixelArray)
{
    memcpy( m_CurrentSprite.getPointer(), 
            (uint8_t*)pixelArray, 
            WIFI_SIGNAL_SPRITE_HIGHT * WIFI_SIGNAL_SPRITE_WIDTH * 2 /*memcpy copies bytes so we need to multiply by 2 for 16bbp*/);
}

void WiFiSignal::SignalStrength(int8_t RSSI)
{
    if((WIFI_SIGNAL_VERY_GOOD <= RSSI))
    {
        if(m_LastRSSI != WIFI_SIGNAL_VERY_GOOD)
        {
            setBackground(WiFiSignal4.pixel_data);
            m_LastRSSI = WIFI_SIGNAL_VERY_GOOD;
        }
    }
    else if(WIFI_SIGNAL_GOOD <= RSSI)
    {
        if(m_LastRSSI != WIFI_SIGNAL_GOOD)
        {
            setBackground(WiFiSignal3.pixel_data);
            m_LastRSSI = WIFI_SIGNAL_GOOD;
        }
    }
    else if(WIFI_SIGNAL_LOW <= RSSI)
    {
        if(m_LastRSSI != WIFI_SIGNAL_LOW)
        {
            setBackground(WiFiSignal2.pixel_data);
            m_LastRSSI = WIFI_SIGNAL_LOW;
        }
    }
    else
    {
        if(m_LastRSSI != WIFI_SIGNAL_VERY_LOW)
        {
            setBackground(WiFiSignal1.pixel_data);
            m_LastRSSI = WIFI_SIGNAL_VERY_LOW;
        }
    }
}

void WiFiSignal::PrintText()
{
    SignalStrength(WiFi.RSSI());
    m_CurrentSprite.pushSprite(m_SpritePossition.x, m_SpritePossition.y, 0x07E0U);
}

void WiFiSignal::ForcePrintText()
{
    PrintText();
}