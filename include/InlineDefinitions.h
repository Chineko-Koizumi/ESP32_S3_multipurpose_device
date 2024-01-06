#ifndef INLINEDEFINITIONS_H
#define INLINEDEFINITIONS_H

#include "inttypes.h"

//-----------------------------------------------
//-----------Screen related consts---------------
//-----------------------------------------------

  static const uint16_t MAX_IMAGE_WIDTH  = 240U;
  static const uint16_t MAX_IMAGE_HIGHT  = 320U;

  static const uint8_t FONT_SIZE_1       = 1U;
  static const uint8_t FONT_SIZE_2       = 2U;
  static const uint8_t FONT_SIZE_3       = 3U;
  static const uint8_t FONT_SIZE_4       = 4U;
  static const uint8_t MAX_LINES_OF_TEXT = 19U; // Maximal value of lines to show on screen
  static const uint8_t MAX_TEXT_LENGTH   = 20U; //19 + '/0' characters is maximal for one line in 2x font size

//-----------------------------------------------

//-----------------------------------------------
//----------Detector related consts--------------
//-----------------------------------------------

  static const uint16_t STACK_SIZE_BME680                   = 2500U;      // Stack size for BME680 detector task in ESP32 words (4 bytes per word)
  static const uint32_t STATE_SAVE_PERIOD	                  = 21600000U;  // 360 minutes - 4 times a day
  static const uint32_t LOW_POWER_MODE_SENSOR_READ_PERIOD	  = 3000;       //3 sec
  static const float    TEMPERATURE_OFFSET                  = 4.0f;
  static const int8_t   PRESSURE_OFFSET                     = 33;
  static const int8_t   HUMIDITY_OFFSET                     = -2; 
  static const uint16_t IAQ_CONFIG_ITEMS_COUNT              = 462U;
  
//-----------------------------------------------

struct MyCoordinates
{
  int16_t x;
  int16_t y;
};

#endif