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

  static const uint8_t  MAX_LINES_OF_TEXT       = 19U; // Maximal value of lines to show on screen
  static const uint8_t  MAX_TEXT_SCREEN_LENGTH  = 20U; //19 + '/0' characters is maximal for one line in 2x font size
  static const uint32_t PIXEL_ARRAY_SIZE        = 153600U;

  static const bool CUSTOM_FONT   = false;
  static const bool DEFAULT_FONT  = true;

  static const uint8_t WIFI_SIGNAL_SPRITE_WIDTH = 40U;
  static const uint8_t WIFI_SIGNAL_SPRITE_HIGHT = 40U;

enum RSSI_READING
{
  WIFI_SIGNAL_VERY_GOOD = -60,
  WIFI_SIGNAL_GOOD      = -70,
  WIFI_SIGNAL_LOW       = -80,
  WIFI_SIGNAL_VERY_LOW  = -90
};
  
struct Coordinates
{
  int16_t x;
  int16_t y;
};

//-----------------------------------------------

//-----------------------------------------------
//-----------Menu related consts---------------
//-----------------------------------------------
enum MENU_INDEX
{
  MENU_MAIN = 0U
};

static const uint8_t MENU_INDEX_VAR = MENU_MAIN; //default values is main menu

//-----------------------------------------------

//-----------------------------------------------
//----------Detector related consts--------------
//-----------------------------------------------

  static const uint16_t STACK_SIZE_BME680                   = 5000U;      // Stack size for BME680 detector task in ESP32 words (4 bytes per word)
  static const uint32_t STATE_SAVE_PERIOD	                  = 21600000U;  // 360 minutes - 4 times a day
  static const uint32_t LOW_POWER_MODE_SENSOR_READ_PERIOD	  = 3000U;      //3 sec
  static const float    TEMPERATURE_OFFSET                  = 4.0f;
  static const int8_t   PRESSURE_OFFSET                     = 33U;
  static const int8_t   HUMIDITY_OFFSET                     = -2; 
  static const uint16_t IAQ_CONFIG_ITEMS_COUNT              = 462U;
  static const uint8_t  IAQ_RGB_LED_GPIO                    = 48U;
  
//-----------------------------------------------

//-----------------------------------------------
//-------------SD related consts-----------------
//-----------------------------------------------

static const bool ONE_BIT_MODE = true;
  
//-----------------------------------------------

//-----------------------------------------------
//-------------RTC related consts----------------
//-----------------------------------------------

static const uint16_t STACK_SIZE_RTC  = 5000U;      // Stack size for FTP task in ESP32 words (4 bytes per word)
static const uint16_t ONE_SECOND_TICK = 999U;       // 999 milis to ensure at least one update per second
  
//-----------------------------------------------

//-----------------------------------------------
//-------------BMP related consts----------------
//-----------------------------------------------

static const uint16_t STACK_SIZE_BMP = 39650U;      // Stack size for FTP task in ESP32 words (4 bytes per word)
  
//-----------------------------------------------

//-----------------------------------------------
//-------------FTP related consts----------------
//-----------------------------------------------

static const uint16_t STACK_SIZE_FTP = 5000U;      // Stack size for FTP task in ESP32 words (4 bytes per word)
  
//-----------------------------------------------

//-----------------------------------------------
//------------WiFi related consts----------------
//-----------------------------------------------

static const char* SSID      = "";
static const char* PASSWORD  = "";
  
//-----------------------------------------------

//-----------------------------------------------
//------------I2C related consts-----------------
//-----------------------------------------------

static const uint8_t I2C_SDA  = 1;
static const uint8_t I2C_SCL  = 38;
  
//-----------------------------------------------

#endif