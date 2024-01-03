#ifndef INLINEDEFINITIONS_H
#define INLINEDEFINITIONS_H

#include "inttypes.h"

//-----------------------------------------------
//-----------Screen related consts---------------
//-----------------------------------------------

  static const uint16_t MAX_IMAGE_WIDTH   = 240U;
  static const uint16_t MAX_IMAGE_HIGHT   = 320U;

  static const uint8_t  MAX_LINES_OF_TEXT = 19U; // Maximal value of lines to show on screen

  static const uint8_t FONT_SIZE_2        = 2U;
  static const uint8_t MAX_TEXT_LENGTH    = 20; //19 + '/0' characters is maximal for one line in 2x font size

//-----------------------------------------------

//-----------------------------------------------
//----------Detector related consts--------------
//-----------------------------------------------

  static const float SEALEVELPRESSURE_HPA = 1013.25f;

//-----------------------------------------------

struct MyCoordinates
{
  int16_t x;
  int16_t y;

};

#endif