#ifndef DISPLAYCANVASSEQUENCE_H
#define DISPLAYCANVASSEQUENCE_H

#include "lvgl/lvgl.h"
#include "Display/DisplayWiFiList.h"

class DisplayCanvasSequenceBase
{ 
protected:
    lv_obj_t *m_pImage{nullptr};
    lv_image_dsc_t **m_aBuffersArray{nullptr};
    uint8_t m_SequenceCount{0U};
    uint8_t m_AddedBuffersIndex{0U};

public:
    virtual ~DisplayCanvasSequenceBase()  = 0;

    virtual void SetPossition(int16_t posX, int16_t posY) = 0;
    virtual void AddBuffer(lv_image_dsc_t* pBuffer) = 0;
    virtual void SetFrame(uint8_t frameNumber) = 0;
};

class DisplayCanvasSequenceWiFiSignal : public DisplayCanvasSequenceBase
{
private:
    enum
    {
        NO_SIGNAL = 0U,
        SIGNAL_LOW,
        SIGNAL_MEDIUM,
        SIGNAL_GOOD,
        SIGNAL_VERY_GOOD
    };

    int8_t m_LastRSSI{0};

    virtual void SetPossition(int16_t posX, int16_t posY) override {};
    virtual void AddBuffer(lv_image_dsc_t* pBuffer) override;
    virtual void SetFrame(uint8_t frameNumber) override;

    static void click_event_handler(lv_event_t * e);

public:
    DisplayCanvasSequenceWiFiSignal(int16_t posX, int16_t posY);
    ~DisplayCanvasSequenceWiFiSignal();

    void SetSignalStrength(int8_t RSSI);
};

#endif