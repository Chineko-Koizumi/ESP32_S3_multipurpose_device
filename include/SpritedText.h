#ifndef SPRITEDTEXT_H
#define SPRITEDTEXT_H

#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library

#include "InlineDefinitions.h"
#include "FreeFonts.h"

class SpritedTextBase
{
protected:
    TFT_eSPI*       m_pScreen;
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

    void setCoordinates(const MyCoordinates& coords);
};

class SpritedText : public SpritedTextBase
{
private:
    char m_aText[MAX_TEXT_LENGTH];

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

    void setCString(char cString[]);
    char* getCharArrayPtr();
    void printText();
};

#endif