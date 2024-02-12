#ifndef INLINEDEFINITIONS_H
#define INLINEDEFINITIONS_H

#include "inttypes.h"

//-----------------------------------------------
//-----------Screen related consts---------------
//-----------------------------------------------

  static const uint16_t MAX_IMAGE_WIDTH  = 240U;
  static const uint16_t MAX_IMAGE_HIGHT  = 320U;

  static const uint32_t DRAW_BUF_SIZE = MAX_IMAGE_WIDTH * MAX_IMAGE_HIGHT * 2 / 5;

  static const uint8_t WIFI_SIGNAL_SPRITE_WIDTH = 40U;
  static const uint8_t WIFI_SIGNAL_SPRITE_HIGHT = 40U;

//-----------------------------------------------

//-----------------------------------------------
//----------Detector related consts--------------
//-----------------------------------------------

  static const uint32_t STATE_SAVE_PERIOD	                  = 21600000U;  // 360 minutes - 4 times a day
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
//-------------FTP related consts----------------
//-----------------------------------------------

static const uint16_t STACK_SIZE_FTP = 5000U;      // Stack size for FTP task in ESP32 words (4 bytes per word)
static const char* FTP_LOGIN     = "";
static const char* FTP_PASSWORD  = "";
  
//-----------------------------------------------

//-----------------------------------------------
//------------WiFi related consts----------------
//-----------------------------------------------

static const char* WIFI_SSID      = "";
static const char* WIFI_PASSWORD  = "";
  
//-----------------------------------------------

//-----------------------------------------------
//------------I2C related consts-----------------
//-----------------------------------------------

static const uint8_t I2C_SDA  = 8U;
static const uint8_t I2C_SCL  = 9U;

static const uint16_t ONE_SECOND_TICK = 1000U;
static const uint16_t STACK_SIZE_I2C  = 10000U;      // Stack size for FTP task in ESP32 words (4 bytes per word)

//-----------------------------------------------

//===============================================
//===============================================
//==============Other Definitions================
//===============================================
//===============================================

enum RSSI_READING
{
  WIFI_SIGNAL_VERY_GOOD = -60,
  WIFI_SIGNAL_GOOD      = -70,
  WIFI_SIGNAL_LOW       = -80,
  WIFI_SIGNAL_VERY_LOW  = -90
};

enum MENU_INDEX
{
  MENU_MAIN = 0U,
  MENU_WIFI = 1U
};
  
#endif