#include "Styles.h"

Styles::LambdaPointer Styles::s_aInitPointers[STYLE_LAST];
lv_style_t Styles::s_aStyles[STYLE_LAST];

void Styles::Init()
{
  /////////////////////////
  ///STYLE_DEFAULT_FRAME///
  /////////////////////////

  s_aInitPointers[STYLE_DEFAULT_FRAME] =  [](lv_style_t* pStyle)
  {
    lv_style_set_bg_color(pStyle, lv_color_white());
    lv_style_set_bg_opa(  pStyle, 255);

    lv_style_set_border_width(  pStyle, 2);
    lv_style_set_border_color(  pStyle, lv_color_black());
    lv_style_set_radius(        pStyle, 0);

    lv_style_set_pad_all(       pStyle, 0);
    lv_style_set_margin_bottom( pStyle, 0);
    lv_style_set_margin_left(   pStyle, 0);
    lv_style_set_margin_right(  pStyle, 0);
    lv_style_set_margin_top(    pStyle, 0);
    
    lv_style_set_text_color(    pStyle, lv_color_black());
  };

  for (size_t i = 0; i < STYLE_LAST; i++)
  {
    lv_style_init(&s_aStyles[i]);
    s_aInitPointers[i](&s_aStyles[i]);
  }
}