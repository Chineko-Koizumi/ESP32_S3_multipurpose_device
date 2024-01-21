#include <SPI.h>
#include <TFT_eSPI.h> 

#include "Sprite/ISpriteObserver.h"

#include "InlineDefinitions.h"


class WiFiSignal: public ISpriteObserver
{
private:
    TFT_eSprite m_CurrentSprite;
    Coordinates m_SpritePossition;

    RSSI_READING m_LastRSSI;
    void setBackground(uint16_t *pixelArray);
    void SignalStrength(int8_t RSSI);

public:
    WiFiSignal(TFT_eSPI *pScreen, const Coordinates& spritePossition);
    ~WiFiSignal();

    void CreateSprite()                         override;
    void PrintSprite()                          override;
    void ForcePrintSprite()                     override;
    void setSpriteBackground(TFT_eSPI* pScreen) override{}; //not used
};

