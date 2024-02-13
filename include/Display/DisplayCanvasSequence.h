#ifndef DISPLAYCANVASSEQUENCE_H
#define DISPLAYCANVASSEQUENCE_H

#include "lvgl/lvgl.h"



class DisplayCanvasSequence
{
private:
    lv_obj_t *m_pCanvas{nullptr};

public:
    DisplayCanvasSequence(int16_t posX, int16_t posY);
    ~DisplayCanvasSequence();
};

#endif