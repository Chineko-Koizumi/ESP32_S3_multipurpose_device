#include "Display/DisplayLabel.h"

#pragma region DisplayLabelBase

DisplayLabelBase::~DisplayLabelBase()
{
}

#pragma endregion DisplayLabelBase

#pragma region DisplayLabel

DisplayLabel::DisplayLabel(SemaphoreHandle_t *renderingMutex, int16_t posX, int16_t posY )
{
    DisplayLabelBase::m_pRenderingMutex = renderingMutex;
    DisplayLabelBase::m_pLabel = lv_label_create( lv_scr_act() );

    lv_obj_align(m_pLabel, LV_ALIGN_TOP_LEFT, posX, posY );
    lv_obj_add_style(m_pLabel, &Styles::s_aStyles[Styles::STYLE_DEFAULT_FRAME], 0);

    lv_obj_set_style_bg_color(  m_pLabel, lv_color_hex(0x115588), 0);
    lv_obj_set_style_text_font( m_pLabel, &lv_font_montserrat_32, 0);
}

DisplayLabel::~DisplayLabel(){}

void DisplayLabel::SetLabelText(const char *pText )
{
    if( xSemaphoreTake( *m_pRenderingMutex, portMAX_DELAY ) == pdTRUE )
    {
        lv_label_set_text(m_pLabel, pText);

        xSemaphoreGive(  *m_pRenderingMutex);
    }
}

#pragma endregion DisplayLabel

#pragma region DisplayLabelIAQ

DisplayLabelIAQ::DisplayLabelIAQ(SemaphoreHandle_t *renderingMutex, int16_t posX, int16_t posY )
{
    DisplayLabelBase::m_pRenderingMutex = renderingMutex;

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

    DisplayLabelBase::m_pLabel = lv_label_create( lv_scr_act() );
    lv_obj_align(m_pLabel, LV_ALIGN_TOP_LEFT, posX, posY );

    lv_obj_add_style(m_pLabel, &Styles::s_aStyles[Styles::STYLE_DEFAULT_FRAME], 0);

    lv_obj_set_style_bg_color(  m_pLabel, lv_color_hex(m_aIAQRGBColors[0]), 0);
    lv_obj_set_style_text_font( m_pLabel, &lv_font_montserrat_32, 0);

    m_pRGB = new Adafruit_NeoPixel(1, IAQ_RGB_LED_GPIO, NEO_GRBW + NEO_KHZ800);
    m_pRGB->setBrightness(30);
    m_pRGB->setPixelColor(0, 0x000000U);
}

    DisplayLabelIAQ::~DisplayLabelIAQ(){}

void DisplayLabelIAQ::SetLabelText(const char *pText )
{
    if( xSemaphoreTake( *m_pRenderingMutex, portMAX_DELAY ) == pdTRUE )
    {
        lv_label_set_text(m_pLabel, pText);

        xSemaphoreGive(  *m_pRenderingMutex);
    } 
}

void DisplayLabelIAQ::SetIAQValue(float value)
{
    /*
    IAQ Accuracy=0 could either mean:
    BSEC was just started, and the sensor is stabilizing (this lasts normally 5min in LP mode or 20min in ULP mode),
    there was a timing violation (i.e. BSEC was called too early or too late), which should be indicated by a warning/error flag by BSEC,
    
    IAQ Accuracy=1 means the background history of BSEC is uncertain. This typically means the gas sensor data was too stable for BSEC to clearly define its references,
    
    IAQ Accuracy=2 means BSEC found a new calibration data and is currently calibrating,
    
    IAQ Accuracy=3 means BSEC calibrated successfully.
*/
    m_IAQColorIndex = value / IAQ_LEVEL_STEP_VALUE;

    if( m_IAQColorIndex > (IAQ_COLOR_COUNT - 1) ) m_IAQColorIndex = IAQ_COLOR_COUNT - 1;

    if(m_IAQColorIndex != m_LastIAQColorIndex)
    {
        m_pRGB->setPixelColor(0, m_aIAQRGBColors[m_IAQColorIndex]);
        m_pRGB->show();

        if( xSemaphoreTake( *m_pRenderingMutex, portMAX_DELAY ) == pdTRUE )
        {
            lv_obj_set_style_bg_color(m_pLabel, lv_color_hex(m_aIAQRGBColors[m_IAQColorIndex]), 0);

            xSemaphoreGive(  *m_pRenderingMutex);
        }

        m_LastIAQColorIndex = m_IAQColorIndex;
    }
    
    snprintf(m_aCharIAQ, 4U, "%.0f", value);
    SetLabelText(m_aCharIAQ);
}

#pragma endregion DisplayLabelIAQ
