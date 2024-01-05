#ifndef SPRITEDTEXT_H
#define SPRITEDTEXT_H

#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library

#include "InlineDefinitions.h"
#include "FreeFonts.h"

class SpritedTextBase
{
protected:
    char m_aText[MAX_TEXT_LENGTH];

    TFT_eSprite    m_ForegroundSprite;
    MyCoordinates   m_SpritePos;

    uint8_t m_FontSize;
    uint16_t m_FontFGColor;
    uint16_t m_FontBGColor;
    uint16_t m_FontMaskColor;

    uint16_t*    m_pImage;

public:
    SpritedTextBase(TFT_eSPI* ScreenPtr, 
                const MyCoordinates& coords, 
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
    uint16_t m_MaxTextWidth;
    uint16_t m_MaxTextHeight;
    uint16_t m_MaxTextLength;

    uint16_t m_LastTextWidth;
    uint16_t m_CurrentWidth;

    void removeSpriteEndingIfNecessery();

public:
    SpritedText(TFT_eSPI* ScreenPtr, 
                const MyCoordinates& coords,
                uint8_t  maxTextLength,     //excluding '\0'
                uint8_t fontSize, 
                uint16_t fontFGColor, 
                uint16_t fontBGColor, 
                uint16_t fontMaskColor);

    ~SpritedText();

    void printText();
};

class IAQText : public SpritedTextBase
{
private:
    static const uint8_t MAX_IAQ_READING_LENGTH   = 3U;
    static const uint8_t IAQ_COLOR_COUNT          = 8U; 
    static const uint8_t IAQ_LEVEL_STEP_VALUE     = 50U; 

    uint16_t m_TextWidth;
    uint16_t m_TextHeight;

    uint16_t m_IAQReading;
    uint8_t m_IAQColorIndex;

    u_int16_t m_aIAQColors[IAQ_COLOR_COUNT];

public:
    IAQText(TFT_eSPI* ScreenPtr, 
            const MyCoordinates& coords, 
            uint8_t fontSize, 
            uint16_t fontFGColor, 
            uint16_t fontBGColor, 
            uint16_t fontMaskColor);
    ~IAQText();

    void printText();
};

#endif