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
  static const uint8_t MAX_TEXT_SCREEN_LENGTH   = 20U; //19 + '/0' characters is maximal for one line in 2x font size

  static const bool CUSTOM_FONT   = false;
  static const bool DEFAULT_FONT  = true;

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

struct MyCoordinates
{
  int16_t x;
  int16_t y;
};

struct BMPFileHeader 
{
  uint16_t file_type{0x4D42};          // File type always BM which is 0x4D42
  uint32_t file_size{0};               // Size of the file (in bytes)
  uint16_t reserved1{0};               // Reserved, always 0
  uint16_t reserved2{0};               // Reserved, always 0
  uint32_t offset_data{0};             // Start position of pixel data (bytes from the beginning of the file)
} __attribute__((packed));

struct BMPInfoHeader 
{
  uint32_t size{ 0 };                     // Size of this header (in bytes)
  int32_t width{ 0 };                     // width of bitmap in pixels
  int32_t height{ 0 };                    // width of bitmap in pixels
                                          //       (if positive, bottom-up, with origin in lower left corner)
                                          //       (if negative, top-down, with origin in upper left corner)
  uint16_t planes{ 1 };                   // No. of planes for the target device, this is always 1
  uint16_t bit_count{ 0 };                // No. of bits per pixel
  uint32_t compression{ 0 };              // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
  uint32_t size_image{ 0 };               // 0 - for uncompressed images
  int32_t x_pixels_per_meter{ 0 };
  int32_t y_pixels_per_meter{ 0 };
  uint32_t colors_used{ 0 };              // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
  uint32_t colors_important{ 0 };         // No. of colors used for displaying the bitmap. If 0 all colors are required
 }  __attribute__((packed));

struct BMPColorHeader 
{
  uint32_t red_mask{ 0x00ff0000 };         // Bit mask for the red channel
  uint32_t green_mask{ 0x0000ff00 };       // Bit mask for the green channel
  uint32_t blue_mask{ 0x000000ff };        // Bit mask for the blue channel
  uint32_t alpha_mask{ 0xff000000 };       // Bit mask for the alpha channel
  uint32_t color_space_type{ 0x73524742 }; // Default "sRGB" (0x73524742)
  uint32_t unused[16]{ 0 };                // Unused data for sRGB color space
}  __attribute__((packed));

struct BMPPathUpdate
{
  bool newData;
  char path[50];

}  __attribute__((packed));

#endif