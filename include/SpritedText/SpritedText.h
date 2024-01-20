#ifndef SPRITEDTEXT_H
#define SPRITEDTEXT_H

#include <SPI.h>
#include <TFT_eSPI.h> 

#include <Adafruit_NeoPixel.h>

#include "ISpritedTextObserver.h"

#include "InlineDefinitions.h"

class SpritedTextBase : public ISpritedTextObserver
{
protected:
    char m_aText[MAX_TEXT_SCREEN_LENGTH];
    char m_aLastText[MAX_TEXT_SCREEN_LENGTH];

    SemaphoreHandle_t* m_xMutexaTextAccess;

    TFT_eSprite    m_ForegroundSprite;
    TFT_eSprite    m_BackgroundSprite;
    TFT_eSprite    m_FinalSprite;
    MyCoordinates  m_SpritePos;

    uint8_t m_FontSize;
    uint16_t m_FontFGColor;
    uint16_t m_FontBGColor;
    uint16_t m_FontMaskColor;

    uint16_t m_MaxTextWidth;
    uint16_t m_MaxTextHeight;
    uint16_t m_MaxTextLength;

    uint16_t*    m_pImage;

public:
    SpritedTextBase(TFT_eSPI* ScreenPtr,
                    SemaphoreHandle_t* xMutexaTextAccess,  
                    const MyCoordinates& coords,
                    uint8_t maxTextLength,
                    uint8_t fontSize, 
                    uint16_t fontFGColor, 
                    uint16_t fontBGColor, 
                    uint16_t fontMaskColor);

    ~SpritedTextBase();

    void setCString(char cString[]);
    char* getCharArrayPtr();
    void setCoordinates(const MyCoordinates& coords);
};

class SpritedText : public SpritedTextBase
{
private:
    uint16_t m_LastTextWidth;
    uint16_t m_CurrentWidth;

    void RemoveSpriteEndingIfNecessery();

public:
    SpritedText(TFT_eSPI* ScreenPtr,
                SemaphoreHandle_t* xMutexaTextAccess,  
                const MyCoordinates& coords,
                uint8_t  maxTextLength,     //excluding '\0'
                uint8_t fontSize, 
                uint16_t fontFGColor, 
                uint16_t fontBGColor, 
                uint16_t fontMaskColor);

    ~SpritedText();

    void PrintText()                                override;
    void ForcePrintText()                           override;
    void CreateSprite()                             override;
    void setSpriteBackground(TFT_eSPI* ScreenPtr)   override;
};

class IAQText : public SpritedTextBase
{
private:
    static const uint8_t MAX_IAQ_READING_LENGTH   = 3U;
    static const uint8_t IAQ_COLOR_COUNT          = 8U; 
    static const uint8_t IAQ_LEVEL_STEP_VALUE     = 50U; 

    Adafruit_NeoPixel* m_pRGB;

    uint16_t m_IAQReading;
    uint8_t m_IAQColorIndex;
    uint8_t m_LastIAQColorIndex;


    u_int16_t m_aIAQTextColors[IAQ_COLOR_COUNT];
    u_int32_t m_aIAQRGBColors[IAQ_COLOR_COUNT];

public:
    IAQText(TFT_eSPI* ScreenPtr,
            SemaphoreHandle_t* xMutexaTextAccess,
            const MyCoordinates& coords,
            uint8_t  maxTextLength,  
            uint8_t fontSize,  
            uint16_t fontBGColor, 
            uint16_t fontMaskColor);
    ~IAQText();

    void PrintText()                                override;
    void ForcePrintText()                           override;
    void CreateSprite()                             override;
    void setSpriteBackground(TFT_eSPI* ScreenPtr)   override;
};

#endif