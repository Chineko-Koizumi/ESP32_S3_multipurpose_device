#include "SpritedText.h"

SpritedTextBase::SpritedTextBase(   TFT_eSPI* ScreenPtr, 
                                    const MyCoordinates& coords, 
                                    uint8_t fontSize, 
                                    uint16_t fontFGColor, 
                                    uint16_t fontBGColor, 
                                    uint16_t fontMaskColor):
    m_pScreen(ScreenPtr),
    m_ForegroundSprite(TFT_eSprite(ScreenPtr)),
    m_SpritePos(coords),
    m_FontSize(fontSize),
    m_FontFGColor(fontFGColor),
    m_FontBGColor(fontBGColor),
    m_FontMaskColor(fontMaskColor)
{

}

SpritedTextBase::~SpritedTextBase(){}

void SpritedTextBase::setCoordinates(const MyCoordinates& coords)
{
    m_SpritePos = coords;
}

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
        m_aText[i] = 'X';
        m_aText[i+1] = '\0';
    }

    m_MaxTextWidth  = ScreenPtr->textWidth(m_aText) * fontSize;
    m_MaxTextHeight = ScreenPtr->fontHeight() * fontSize;

    m_pCurrentWidth = m_MaxTextWidth;
    m_LastTextWidth = m_MaxTextWidth;

    SpritedTextBase::m_ForegroundSprite.createSprite(m_MaxTextWidth, m_MaxTextHeight);
    SpritedTextBase::m_ForegroundSprite.setTextSize(SpritedTextBase::m_FontSize);
    SpritedTextBase::m_ForegroundSprite.setTextColor(SpritedTextBase::m_FontFGColor, SpritedTextBase::m_FontBGColor, false);

    memset(m_aText, '\0', MAX_TEXT_LENGTH);
}

SpritedText::~SpritedText(){}

void SpritedText::setCString(char* cString)
{
    cString[MAX_TEXT_LENGTH - 1] = '\0';
    strcpy(m_aText, cString);
}

char* SpritedText::getCharArrayPtr()
{
    return m_aText;
}

void SpritedText::printText()
{
    SpritedTextBase::m_ForegroundSprite.setCursor(SpritedTextBase::m_SpritePos.x, SpritedTextBase::m_SpritePos.y);
    removeSpriteEndingIfNecessery();
    SpritedTextBase::m_ForegroundSprite.drawString(m_aText, 0, 0);

    SpritedTextBase::m_ForegroundSprite.pushSprite( SpritedTextBase::m_SpritePos.x, 
                                                    SpritedTextBase::m_SpritePos.y, 
                                                    SpritedTextBase::m_FontMaskColor);
}

void SpritedText::removeSpriteEndingIfNecessery()
{
    m_pCurrentWidth = SpritedTextBase::m_pScreen->textWidth(m_aText) *  SpritedTextBase::m_FontSize;
    
    if(m_LastTextWidth > m_pCurrentWidth)
    {

       /* SpritedTextBase::m_BackgroundSprite.pushSprite( SpritedTextBase::m_SpritePos.x + (int16_t)m_pCurrentWidth, 
                                                SpritedTextBase::m_SpritePos.y,
                                                SpritedTextBase::m_SpritePos.x + (int16_t)m_pCurrentWidth, 
                                                SpritedTextBase::m_SpritePos.y,
                                                m_LastTextWidth - m_pCurrentWidth, 
                                                m_MaxTextHeight);*/ //for some reason this does not work

        SpritedTextBase::m_ForegroundSprite.fillRect(   m_pCurrentWidth, 
                                                        0, 
                                                        m_LastTextWidth - m_pCurrentWidth, 
                                                        m_MaxTextHeight, 
                                                        SpritedTextBase::m_FontBGColor);
    }
    m_LastTextWidth = m_pCurrentWidth;
}