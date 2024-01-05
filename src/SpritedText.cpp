#include "SpritedText.h"

#pragma region SpritedTextBase

SpritedTextBase::SpritedTextBase(   TFT_eSPI* ScreenPtr, 
                                    const MyCoordinates& coords, 
                                    uint8_t fontSize, 
                                    uint16_t fontFGColor, 
                                    uint16_t fontBGColor, 
                                    uint16_t fontMaskColor):
    m_ForegroundSprite(TFT_eSprite(ScreenPtr)),
    m_SpritePos(coords),
    m_FontSize(fontSize),
    m_FontFGColor(fontFGColor),
    m_FontBGColor(fontBGColor),
    m_FontMaskColor(fontMaskColor)
{}

SpritedTextBase::~SpritedTextBase(){}

void SpritedTextBase::setCoordinates(const MyCoordinates& coords)
{
    m_SpritePos = coords;
}

void SpritedTextBase::setCString(char cString[])
{
    strcpy(m_aText, cString);
}

char* SpritedTextBase::getCharArrayPtr()
{
    return m_aText;
}

#pragma endregion SpritedTextBase

#pragma region SpritedText

SpritedText::SpritedText(   TFT_eSPI* ScreenPtr,  
                            const MyCoordinates& coords,
                            uint8_t  maxTextLength, 
                            uint8_t fontSize, 
                            uint16_t fontFGColor, 
                            uint16_t fontBGColor, 
                            uint16_t fontMaskColor):
SpritedTextBase(ScreenPtr, coords, fontSize, fontFGColor, fontBGColor, fontMaskColor),
m_MaxTextLength(maxTextLength)
{
    for (size_t i = 0; (i < MAX_TEXT_LENGTH - 1)/* -1 beacause of '\0' */ && (i < maxTextLength); i++)
    {
        m_aText[i] = 'X';
        m_aText[i+1] = '\0';
    }
    SpritedTextBase::m_ForegroundSprite.setTextSize(SpritedTextBase::m_FontSize);
    SpritedTextBase::m_ForegroundSprite.setTextColor(SpritedTextBase::m_FontFGColor, SpritedTextBase::m_FontBGColor, false);
    SpritedTextBase::m_ForegroundSprite.setFreeFont( /* FMB18 */ FF23 /* FF43 */);

    m_MaxTextWidth  = m_ForegroundSprite.textWidth(m_aText);
    m_MaxTextHeight = m_ForegroundSprite.fontHeight();

    m_CurrentWidth = m_MaxTextWidth;
    m_LastTextWidth = m_MaxTextWidth;

    SpritedTextBase::m_ForegroundSprite.createSprite(m_MaxTextWidth, m_MaxTextHeight);

    memset(m_aText, '\0', MAX_TEXT_LENGTH);
}

SpritedText::~SpritedText(){}

void SpritedText::printText()
{
    removeSpriteEndingIfNecessery();
    SpritedTextBase::m_ForegroundSprite.drawString(m_aText, 0, 6);
    SpritedTextBase::m_ForegroundSprite.pushSprite( SpritedTextBase::m_SpritePos.x, 
                                                    SpritedTextBase::m_SpritePos.y, 
                                                    SpritedTextBase::m_FontMaskColor);
}

void SpritedText::removeSpriteEndingIfNecessery()
{
    m_CurrentWidth = SpritedTextBase::m_ForegroundSprite.textWidth(m_aText);
    
    if(m_LastTextWidth > m_CurrentWidth)
    {

        /* SpritedTextBase::m_BackgroundSprite.pushSprite( SpritedTextBase::m_SpritePos.x + m_CurrentWidth, 
                                                SpritedTextBase::m_SpritePos.y,
                                                SpritedTextBase::m_SpritePos.x + m_CurrentWidth, 
                                                SpritedTextBase::m_SpritePos.y,
                                                m_LastTextWidth - m_CurrentWidth, 
                                                m_MaxTextHeight); */ //for some reason this does not work

        SpritedTextBase::m_ForegroundSprite.fillRect(   m_CurrentWidth, 
                                                        0, 
                                                        m_LastTextWidth - m_CurrentWidth, 
                                                        m_MaxTextHeight, 
                                                        SpritedTextBase::m_FontBGColor);
    }
    m_LastTextWidth = m_CurrentWidth;
}

#pragma endregion SpritedText

#pragma region IAQText

IAQText::IAQText(   TFT_eSPI* ScreenPtr, 
                    const MyCoordinates& coords, 
                    uint8_t fontSize, 
                    uint16_t fontFGColor, 
                    uint16_t fontBGColor, 
                    uint16_t fontMaskColor):
SpritedTextBase(ScreenPtr, coords, fontSize, fontFGColor, fontBGColor, fontMaskColor)
{
    m_aIAQColors[0] = 0x0720U; // 0-50      Light green --> Excellent:              Pure air
    m_aIAQColors[1] = 0x968AU; // 51-100    Green -->       Good:                   No irritation or impact on well-being
    m_aIAQColors[2] = 0xFFE0U; // 101-150   Yellow -->      Lightly polluted:       Reduction on well-being possible
    m_aIAQColors[3] = 0xFBE0U; // 151-200   Orange -->      Moderately polluted:    More significant irritation possible 
    m_aIAQColors[4] = 0xF880U; // 201-250   Red -->         Heavily polluted:       Exposition might lead to effect like headache depending on type of VOCs
    m_aIAQColors[5] = 0x9869U; // 251-300   Purple -->      Severely polluted:      More severe health issue possible if harmfull VOCs are present
    m_aIAQColors[6] = 0x9869U; // 301-351   Purple -->      Severely polluted:      More severe health issue possible if harmfull VOCs are present
    m_aIAQColors[7] = 0x6180U; // >351      Brown -->       Extremely polluted:     Run!!! 

    SpritedTextBase::m_aText[0] = '9';
    SpritedTextBase::m_aText[1] = '9';
    SpritedTextBase::m_aText[2] = '9';
    SpritedTextBase::m_aText[3] = '\0';

    SpritedTextBase::m_ForegroundSprite.setTextSize(SpritedTextBase::m_FontSize);
    SpritedTextBase::m_ForegroundSprite.setTextColor(SpritedTextBase::m_FontFGColor, SpritedTextBase::m_FontBGColor, false);
    SpritedTextBase::m_ForegroundSprite.setFreeFont( /* FMB18 */ FF23 /* FF43 */);

    m_TextWidth     = m_ForegroundSprite.textWidth(m_aText);
    m_TextHeight    = m_ForegroundSprite.fontHeight();

    SpritedTextBase::m_ForegroundSprite.createSprite(m_TextWidth, m_TextHeight);
}

IAQText::~IAQText()
{
    
}

void IAQText::printText()
{
    m_IAQReading = atoi(SpritedTextBase::m_aText);
    
    m_IAQColorIndex = m_IAQReading / IAQ_LEVEL_STEP_VALUE;

    if( m_IAQColorIndex > (IAQ_COLOR_COUNT - 1) )
    {
       m_IAQColorIndex = IAQ_COLOR_COUNT - 1;
    }

    SpritedTextBase::m_FontFGColor = m_aIAQColors[m_IAQColorIndex];

    SpritedTextBase::m_ForegroundSprite.setTextColor(SpritedTextBase::m_FontFGColor, SpritedTextBase::m_FontBGColor, false);
    SpritedTextBase::m_ForegroundSprite.fillRect(0, 0, m_TextWidth, m_TextHeight, SpritedTextBase::m_FontBGColor);
    SpritedTextBase::m_ForegroundSprite.drawString(SpritedTextBase::m_aText, 0, 6);
    SpritedTextBase::m_ForegroundSprite.pushSprite( SpritedTextBase::m_SpritePos.x, 
                                                    SpritedTextBase::m_SpritePos.y, 
                                                    SpritedTextBase::m_FontMaskColor);
}

#pragma endregion IAQText