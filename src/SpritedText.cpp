#include "SpritedText.h"

#pragma region SpritedTextBase

SpritedTextBase::SpritedTextBase(   TFT_eSPI* ScreenPtr,
                                    SemaphoreHandle_t* xMutexaTextAccess,  
                                    const MyCoordinates& coords,
                                    uint8_t maxTextLength,
                                    uint8_t fontSize, 
                                    uint16_t fontFGColor, 
                                    uint16_t fontBGColor, 
                                    uint16_t fontMaskColor):
    m_ForegroundSprite(TFT_eSprite(ScreenPtr)),
    m_BackgroundSprite(TFT_eSprite(ScreenPtr)),
    m_xMutexaTextAccess(xMutexaTextAccess),
    m_FinalSprite(TFT_eSprite(ScreenPtr)),
    m_SpritePos(coords),
    m_MaxTextLength(maxTextLength),
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
                            SemaphoreHandle_t* xMutexaTextAccess,  
                            const MyCoordinates& coords,
                            uint8_t  maxTextLength, 
                            uint8_t fontSize, 
                            uint16_t fontFGColor, 
                            uint16_t fontBGColor, 
                            uint16_t fontMaskColor):
SpritedTextBase(ScreenPtr, xMutexaTextAccess, coords, maxTextLength, fontSize, fontFGColor, fontBGColor, fontMaskColor)
{
    for (size_t i = 0; (i < MAX_TEXT_SCREEN_LENGTH - 1)/* -1 beacause of '\0' */ && (i < m_MaxTextLength); i++)
    {
        m_aText[i]          = 'X';
        m_aText[i+1]        = '\0';

        m_aLastText[i]      = 'X';
        m_aLastText[i+1]    = '\0';
    }

    m_ForegroundSprite.setTextSize(m_FontSize);
    m_ForegroundSprite.setTextColor(m_FontFGColor, m_FontBGColor, false);

    m_MaxTextWidth  = m_ForegroundSprite.textWidth(m_aText);
    m_MaxTextHeight = m_ForegroundSprite.fontHeight();

    m_CurrentWidth = m_MaxTextWidth;
    m_LastTextWidth = m_MaxTextWidth;

    memset(m_aText,     '\0', MAX_TEXT_SCREEN_LENGTH);
    memset(m_aLastText, '\0', MAX_TEXT_SCREEN_LENGTH);
}

SpritedText::~SpritedText(){}

void SpritedText::PrintText()
{
    if(strcmp(m_aLastText, m_aText))
    {
        RemoveSpriteEndingIfNecessery();

        m_BackgroundSprite.pushToSprite(&m_FinalSprite, 0, 0);

        if( xSemaphoreTake( *m_xMutexaTextAccess, portMAX_DELAY ) == pdTRUE )
        {
            m_ForegroundSprite.drawString(m_aText, 0, 0);
                
            xSemaphoreGive( *m_xMutexaTextAccess);
        }

        m_ForegroundSprite.pushToSprite(&m_FinalSprite, 0, 0, m_FontMaskColor);

        m_FinalSprite.pushSprite(m_SpritePos.x, m_SpritePos.y);

        strcpy(m_aLastText, m_aText); 
    }                            
}

void SpritedText::ForcePrintText()
{
    m_BackgroundSprite.pushToSprite(&m_FinalSprite, 0, 0);

    m_ForegroundSprite.fillRect(0, 0, m_MaxTextWidth, m_MaxTextHeight, m_FontMaskColor);

    if( xSemaphoreTake( *m_xMutexaTextAccess, portMAX_DELAY ) == pdTRUE )
    {
        m_ForegroundSprite.drawString(m_aText, 0, 0);
            
        xSemaphoreGive( *m_xMutexaTextAccess);
    }

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
                    SemaphoreHandle_t* xMutexaTextAccess, 
                    const MyCoordinates& coords,
                    uint8_t  maxTextLength, 
                    uint8_t fontSize, 
                    uint16_t fontBGColor, 
                    uint16_t fontMaskColor):
SpritedTextBase(ScreenPtr, xMutexaTextAccess, coords, maxTextLength, fontSize, 0xFFFFU, fontBGColor, fontMaskColor),
m_IAQColorIndex(0U),
m_LastIAQColorIndex(0U)
{
    m_pRGB = new Adafruit_NeoPixel(1, IAQ_RGB_LED_GPIO, NEO_GRBW + NEO_KHZ800);

    m_pRGB->setBrightness(30);
    m_pRGB->setPixelColor(0, 0x000000U);

    m_aIAQTextColors[0] = 0x0720U; // 0-50      Light green --> Excellent:              Pure air
    m_aIAQTextColors[1] = 0x968AU; // 51-100    Green -->       Good:                   No irritation or impact on well-being
    m_aIAQTextColors[2] = 0xFFE0U; // 101-150   Yellow -->      Lightly polluted:       Reduction on well-being possible
    m_aIAQTextColors[3] = 0xFBE0U; // 151-200   Orange -->      Moderately polluted:    More significant irritation possible 
    m_aIAQTextColors[4] = 0xF880U; // 201-250   Red -->         Heavily polluted:       Exposition might lead to effect like headache depending on type of VOCs
    m_aIAQTextColors[5] = 0x9869U; // 251-300   Purple -->      Severely polluted:      More severe health issue possible if harmfull VOCs are present
    m_aIAQTextColors[6] = 0x9869U; // 301-351   Purple -->      Severely polluted:      More severe health issue possible if harmfull VOCs are present
    m_aIAQTextColors[7] = 0x6180U; // >351      Brown -->       Extremely polluted:     Run!!! 

    m_aIAQRGBColors[0] = 0x00E700U; // 0-50      Light green --> Excellent:              Pure air
    m_aIAQRGBColors[1] = 0x94D252U; // 51-100    Green -->       Good:                   No irritation or impact on well-being
    m_aIAQRGBColors[2] = 0xFFFF00U; // 101-150   Yellow -->      Lightly polluted:       Reduction on well-being possible
    m_aIAQRGBColors[3] = 0xFF7D00U; // 151-200   Orange -->      Moderately polluted:    More significant irritation possible 
    m_aIAQRGBColors[4] = 0xFF1000U; // 201-250   Red -->         Heavily polluted:       Exposition might lead to effect like headache depending on type of VOCs
    m_aIAQRGBColors[5] = 0x9C0C4AU; // 251-300   Purple -->      Severely polluted:      More severe health issue possible if harmfull VOCs are present
    m_aIAQRGBColors[6] = 0x9C0C4AU; // 301-351   Purple -->      Severely polluted:      More severe health issue possible if harmfull VOCs are present
    m_aIAQRGBColors[7] = 0x633100U; // >351      Brown -->       Extremely polluted:     Run!!!

    for (size_t i = 0; (i < MAX_TEXT_SCREEN_LENGTH - 1)/* -1 beacause of '\0' */ && (i < m_MaxTextLength); ++i)
    {
        m_aText[i]          = 'X';
        m_aText[i+1]        = '\0';

        m_aLastText[i]      = 'X';
        m_aLastText[i+1]    = '\0';
    }

    m_ForegroundSprite.setTextSize(m_FontSize);
    m_ForegroundSprite.setTextColor(m_FontFGColor, m_FontMaskColor, false);

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

        if(m_IAQColorIndex != m_LastIAQColorIndex)
        {
            m_pRGB->setPixelColor(0, m_aIAQRGBColors[m_IAQColorIndex]);
            m_pRGB->show();

            m_FontFGColor = m_aIAQTextColors[m_IAQColorIndex];
            m_ForegroundSprite.setTextColor(m_FontFGColor); 

            m_LastIAQColorIndex = m_IAQColorIndex;
        }

        m_ForegroundSprite.fillRect(0, 0, m_MaxTextWidth, m_MaxTextHeight, m_FontMaskColor);

        if( xSemaphoreTake( *m_xMutexaTextAccess, portMAX_DELAY ) == pdTRUE )
        {
            m_ForegroundSprite.drawString(m_aText, 0, 0);
                
            xSemaphoreGive( *m_xMutexaTextAccess);
        }
        
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

        m_pRGB->setPixelColor(0, m_aIAQRGBColors[m_IAQColorIndex]);
        m_pRGB->show();

        m_BackgroundSprite.pushToSprite(&m_FinalSprite, 0, 0);

        m_FontFGColor = m_aIAQTextColors[m_IAQColorIndex];
        m_ForegroundSprite.fillRect(0, 0, m_MaxTextWidth, m_MaxTextHeight, m_FontMaskColor);
        m_ForegroundSprite.setTextColor(m_FontFGColor);
           
        if( xSemaphoreTake( *m_xMutexaTextAccess, portMAX_DELAY ) == pdTRUE )
        {
            m_ForegroundSprite.drawString(m_aText, 0, 0);
                
            xSemaphoreGive( *m_xMutexaTextAccess);
        }

        m_ForegroundSprite.pushToSprite(&m_FinalSprite, 0, 0, m_FontMaskColor);

        m_FinalSprite.pushSprite(m_SpritePos.x, m_SpritePos.y); 
}

void IAQText::CreateSprite()
{
    m_pRGB->begin();
    m_pRGB->show();

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