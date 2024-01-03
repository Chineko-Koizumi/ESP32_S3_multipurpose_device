#include "Horo.h"
#include "SpritedText.h"

TFT_eSPI tft  = TFT_eSPI(); // Invoke custom library

SpritedText coordsText(&tft, (uint16_t*)Horo_image.pixel_data, MyCoordinates{10U,5U}, 2U, 0x4FA4U, 0x0U, 0xFFE0);
// Code to run a screen calibration, not needed when calibration values set in setup()
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // Calibrate
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 0);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.println("Touch corners as indicated");

  tft.setTextFont(1);
  tft.println();

  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

  Serial.println(); Serial.println();
  Serial.println("// Use this calibration code in setup():");
  Serial.print("  uint16_t calData[5] = ");
  Serial.print("{ ");

  for (uint8_t i = 0; i < 5; i++)
  {
    Serial.print(calData[i]);
    if (i < 4) Serial.print(", ");
  }

  Serial.println(" };");
  Serial.print("  tft.setTouch(calData);");
  Serial.println(); Serial.println();

  tft.fillScreen(TFT_BLACK);
  
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Calibration complete!");
  tft.println("Calibration code sent to Serial port.");

  delay(4000);
}

void setup() 
{
  // Use serial port
  Serial.begin(115200);

  // Initialise the TFT screen
  tft.init();

  uint16_t calData[5] = { 242, 3583, 325, 3617, 4 };
  tft.setTouch(calData);

  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true); 
  tft.pushImage(0, 0, MAX_IMAGE_WIDTH, MAX_IMAGE_HIGHT, (uint16_t *)Horo_image.pixel_data);
}

//------------------------------------------------------------------------------------------
bool pressed;
uint16_t x,y;
void loop(void) 
{
  // Pressed will be set true is there is a valid touch on the screen
  pressed = tft.getTouch(&x, &y);

  // Draw a white spot at the detected coordinates
  if (pressed) 
  {
    sprintf(coordsText.getCharArrayPtr(), "x=[%u] y=[%u]", x, y);
    coordsText.printText();

    Serial.print("x,y = ");
    Serial.print(x);
    Serial.print(",");
    Serial.println(y);
  }
}


