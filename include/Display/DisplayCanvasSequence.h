#ifndef DISPLAYCANVASSEQUENCE_H
#define DISPLAYCANVASSEQUENCE_H

#include "lvgl/lvgl.h"

class DisplayCanvasSequenceBase
{
protected:
    lv_obj_t *m_pCanvas{nullptr};
    uint8_t **m_aBuffersArray{nullptr};
    uint8_t m_SequenceCount{0U};
    uint8_t m_AddedBuffersIndex{0U};

public:
    virtual ~DisplayCanvasSequenceBase()  = 0;

    virtual void SetPossition(int16_t posX, int16_t posY) = 0;
    virtual void AddBuffer(uint8_t* pBuffer) = 0;
    virtual void SetFrame(uint8_t frameNumber) = 0;
};

class DisplayCanvasSequenceWiFiSignal : public DisplayCanvasSequenceBase
{
private:
    int8_t m_LastRSSI{0};

    virtual void SetPossition(int16_t posX, int16_t posY) override {};
    virtual void AddBuffer(uint8_t* pBuffer) override;
    virtual void SetFrame(uint8_t frameNumber) override;

public:
    DisplayCanvasSequenceWiFiSignal(int16_t posX, int16_t posY);
    ~DisplayCanvasSequenceWiFiSignal();

    void SetSignalStrength(int8_t RSSI);
};

#endif