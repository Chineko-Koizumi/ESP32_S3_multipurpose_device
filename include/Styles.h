#ifndef INLINESTYLES_H
#define INLINESTYLES_H

#include "lvgl/lvgl.h"
class Styles
{

private:
  Styles() = default;
  ~Styles() = default;

public:
  enum
  {
    STYLE_DEFAULT_FRAME = 0U,
    STYLE_LAST
  };

  typedef void(*LambdaPointer)(lv_style_t*);
  static LambdaPointer s_aInitPointers[STYLE_LAST];

  static lv_style_t s_aStyles[STYLE_LAST];

  static void Init();

};

#endif