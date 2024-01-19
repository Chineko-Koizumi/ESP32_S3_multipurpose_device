#include "SpritedText.h"

#pragma region SpritedTextBase

SpritedTextBase::SpritedTextBase(   TFT_eSPI* ScreenPtr, 
                                    const MyCoordinates& coords, 
                                    uint8_t fontSize, 
                                    uint16_t fontFGColor, 
                                    uint16_t fontBGColor, 
                                    uint16_t fontMaskColor):
    m_ForegroundSprite(TFT_eSprite(ScreenPtr)),
    m_BackgroundSprite(TFT_eSprite(ScreenPtr)),
    m_FinalSprite(TFT_eSprite(ScreenPtr)),
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
SpritedTextBase(ScreenPtr, coords, fontSize, fontFGColor, fontBGColor, fontMaskColor)
{
    for (size_t i = 0; (i < MAX_TEXT_LENGTH - 1)/* -1 beacause of '\0' */ && (i < maxTextLength); i++)
    {
        m_aText[i]          = 'X';
        m_aText[i+1]        = '\0';

        m_aLastText[i]      = 'X';
        m_aLastText[i+1]    = '\0';
    }

    m_ForegroundSprite.setTextSize(m_FontSize);
    m_ForegroundSprite.setTextColor(m_FontFGColor, m_FontBGColor, false);
    m_ForegroundSprite.setFreeFont( /* FMB18 */ FF23 /* FF43 */);

    m_MaxTextLength = maxTextLength;
    m_MaxTextWidth  = m_ForegroundSprite.textWidth(m_aText);
    m_MaxTextHeight = m_ForegroundSprite.fontHeight();

    m_CurrentWidth = m_MaxTextWidth;
    m_LastTextWidth = m_MaxTextWidth;

    memset(m_aText,     '\0', MAX_TEXT_LENGTH);
    memset(m_aLastText, '\0', MAX_TEXT_LENGTH);
}

SpritedText::~SpritedText(){}

void SpritedText::PrintText()
{
    if(strcmp(m_aLastText, m_aText))
    {
        RemoveSpriteEndingIfNecessery();

        m_BackgroundSprite.pushToSprite(&m_FinalSprite, 0, 0);

        m_ForegroundSprite.drawString(m_aText, 0, 0);
        m_ForegroundSprite.pushToSprite(&m_FinalSprite, 0, 0, m_FontMaskColor);

        m_FinalSprite.pushSprite(m_SpritePos.x, m_SpritePos.y);

        strcpy(m_aLastText, m_aText); 
    }                            
}

void SpritedText::ForcePrintText()
{
    m_BackgroundSprite.pushToSprite(&m_FinalSprite, 0, 0);

    m_ForegroundSprite.fillRect(0, 0, m_MaxTextWidth, m_MaxTextHeight, m_FontMaskColor);
    m_ForegroundSprite.drawString(m_aText, 0, 0);
    m_ForegroundSprite.pushToSprite(&m_FinalSprite, 0, 0, m_FontMaskColor);

    m_FinalSprite.pushSprite(m_SpritePos.x, m_SpritePos.y);
}

void SpritedText::RemoveSpriteEndingIfNecessery()
{
    m_CurrentWidth = m_ForegroundSprite.textWidth(m_aText);
    
    if(m_LastTextWidth > m_CurrentWidth)
    {
        m_ForegroundSprite.fillRect(m_CurrentWidth, 
                                    0, 
                                    m_LastTextWidth - m_CurrentWidth, 
                                    m_MaxTextHeight, 
                                    m_FontMaskColor);
    }
    m_LastTextWidth = m_CurrentWidth;
}

 void SpritedText::CreateSprite()//is needed to create this way (instead of in constructor) because ESP32 FreeRTOS implamentation does not allow large data alllocation before call_start_cpu()
 {
    m_ForegroundSprite.createSprite(m_MaxTextWidth, m_MaxTextHeight);
    m_BackgroundSprite.createSprite(m_MaxTextWidth, m_MaxTextHeight);
    m_FinalSprite.createSprite(m_MaxTextWidth, m_MaxTextHeight);

    m_ForegroundSprite.setColorDepth(16);
    m_BackgroundSprite.setColorDepth(16);
    m_FinalSprite.setColorDepth(16);
 }

void SpritedText::setSpriteBackground(TFT_eSPI* ScreenPtr)
{
    ScreenPtr->readRect(m_SpritePos.x, 
                        m_SpritePos.y,
                        m_MaxTextWidth, 
                        m_MaxTextHeight,
                        (uint16_t*) m_BackgroundSprite.getPointer());
}

#pragma endregion SpritedText

#pragma region IAQText

IAQText::IAQText(   TFT_eSPI* ScreenPtr, 
                    const MyCoordinates& coords, 
                    uint8_t fontSize, 
                    uint16_t fontBGColor, 
                    uint16_t fontMaskColor):
SpritedTextBase(ScreenPtr, coords, fontSize, 0xFFFFU, fontBGColor, fontMaskColor)
{
    m_aIAQColors[0] = 0x0720U; // 0-50      Light green --> Excellent:              Pure air
    m_aIAQColors[1] = 0x968AU; // 51-100    Green -->       Good:                   No irritation or impact on well-being
    m_aIAQColors[2] = 0xFFE0U; // 101-150   Yellow -->      Lightly polluted:       Reduction on well-being possible
    m_aIAQColors[3] = 0xFBE0U; // 151-200   Orange -->      Moderately polluted:    More significant irritation possible 
    m_aIAQColors[4] = 0xF880U; // 201-250   Red -->         Heavily polluted:       Exposition might lead to effect like headache depending on type of VOCs
    m_aIAQColors[5] = 0x9869U; // 251-300   Purple -->      Severely polluted:      More severe health issue possible if harmfull VOCs are present
    m_aIAQColors[6] = 0x9869U; // 301-351   Purple -->      Severely polluted:      More severe health issue possible if harmfull VOCs are present
    m_aIAQColors[7] = 0x6180U; // >351      Brown -->       Extremely polluted:     Run!!! 

    m_aText[0] = '9';
    m_aText[1] = '9';
    m_aText[2] = '9';
    m_aText[3] = '\0';

    m_aLastText[0] = '9';
    m_aLastText[1] = '9';
    m_aLastText[2] = '9';
    m_aLastText[3] = '\0';

    m_ForegroundSprite.setTextSize(m_FontSize);
    m_ForegroundSprite.setTextColor(m_FontFGColor, m_FontMaskColor, false);
    m_ForegroundSprite.setFreeFont( /* FMB18 */ FF23 /* FF43 */);

    m_MaxTextWidth     = m_ForegroundSprite.textWidth(m_aText);
    m_MaxTextHeight    = m_ForegroundSprite.fontHeight();
}

IAQText::~IAQText()
{
    
}

void IAQText::PrintText()
{
    if(strcmp(m_aLastText, m_aText))
    {
        m_IAQReading = atoi(m_aText);
        m_IAQColorIndex = m_IAQReading / IAQ_LEVEL_STEP_VALUE;

        if( m_IAQColorIndex > (IAQ_COLOR_COUNT - 1) ) m_IAQColorIndex = IAQ_COLOR_COUNT - 1;

        m_BackgroundSprite.pushToSprite(&m_FinalSprite, 0, 0);

        m_FontFGColor = m_aIAQColors[m_IAQColorIndex];
        m_ForegroundSprite.fillRect(0, 0, m_MaxTextWidth, m_MaxTextHeight, m_FontMaskColor);
        m_ForegroundSprite.setTextColor(m_FontFGColor);    
        m_ForegroundSprite.drawString(m_aText, 0, 0);
        m_ForegroundSprite.pushToSprite(&m_FinalSprite, 0, 0, m_FontMaskColor);

        m_FinalSprite.pushSprite(m_SpritePos.x, m_SpritePos.y); 

        strcpy(m_aLastText, m_aText); 
    }
}

void IAQText::ForcePrintText()
{
        m_IAQReading = atoi(m_aText);
        m_IAQColorIndex = m_IAQReading / IAQ_LEVEL_STEP_VALUE;

        if( m_IAQColorIndex > (IAQ_COLOR_COUNT - 1) ) m_IAQColorIndex = IAQ_COLOR_COUNT - 1;

        m_BackgroundSprite.pushToSprite(&m_FinalSprite, 0, 0);

        m_FontFGColor = m_aIAQColors[m_IAQColorIndex];
        m_ForegroundSprite.fillRect(0, 0, m_MaxTextWidth, m_MaxTextHeight, m_FontMaskColor);
        m_ForegroundSprite.setTextColor(m_FontFGColor);    
        m_ForegroundSprite.drawString(m_aText, 0, 0);
        m_ForegroundSprite.pushToSprite(&m_FinalSprite, 0, 0, m_FontMaskColor);

        m_FinalSprite.pushSprite(m_SpritePos.x, m_SpritePos.y); 
}

void IAQText::CreateSprite()
{
    m_ForegroundSprite.createSprite(m_MaxTextWidth, m_MaxTextHeight);
    m_BackgroundSprite.createSprite(m_MaxTextWidth, m_MaxTextHeight);
    m_FinalSprite.createSprite(m_MaxTextWidth, m_MaxTextHeight);

    m_ForegroundSprite.setColorDepth(16);
    m_BackgroundSprite.setColorDepth(16);
    m_FinalSprite.setColorDepth(16);
}

void IAQText::setSpriteBackground(TFT_eSPI* ScreenPtr)
{
    ScreenPtr->readRect(SpritedTextBase::m_SpritePos.x, 
                SpritedTextBase::m_SpritePos.y,
                m_MaxTextWidth, 
                m_MaxTextHeight,
                (uint16_t*) m_BackgroundSprite.getPointer());
}

#pragma endregion IAQText