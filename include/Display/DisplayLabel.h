#ifndef DISPLAYLABEL_H
#define DISPLAYLABEL_H

#include <Adafruit_NeoPixel.h>

#include "lvgl/lvgl.h"
#include "InlineDefinitions.h"

class DisplayLabelBase
{
protected:
    lv_obj_t    *m_pLabel{nullptr};
    lv_style_t  *m_pStyle{nullptr};

    SemaphoreHandle_t *m_pRenderingMutex{nullptr};

public:
   virtual ~DisplayLabelBase() = 0;

   virtual void SetLabelText(const char *pText ) = 0;
};

class DisplayLabel: public DisplayLabelBase
{

public:
    DisplayLabel(SemaphoreHandle_t *renderingMutex, lv_style_t* pStyle, int16_t posX, int16_t posY );
    ~DisplayLabel();

   void SetLabelText(const char *pText ) override;  
};

class DisplayLabelIAQ : public DisplayLabelBase
{
private:
    static const uint8_t IAQ_COLOR_COUNT          = 8U; 
    static const uint8_t IAQ_LEVEL_STEP_VALUE     = 50U; 

    Adafruit_NeoPixel* m_pRGB;

    lv_style_t m_IAQStyle;

    uint16_t m_aIAQTextColors[IAQ_COLOR_COUNT];
    uint32_t m_aIAQRGBColors[IAQ_COLOR_COUNT];

    char m_aCharIAQ[5];

    uint8_t m_IAQColorIndex{0U}; 
    uint8_t m_LastIAQColorIndex{0U};

    void SetLabelText(const char *pText ) override;

public:
    DisplayLabelIAQ(SemaphoreHandle_t *renderingMutex, int16_t posX, int16_t posY );
    ~DisplayLabelIAQ();

    void SetIAQValue(float value);
};

#endif