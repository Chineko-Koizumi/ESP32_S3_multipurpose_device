#include <WiFi.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#include "Horo.h"         //Background file
#include "SpritedText.h"

TFT_eSPI tft  = TFT_eSPI();

Adafruit_BME680 bme; // I2C

SpritedText Text(&tft, MyCoordinates{6,5}, strlen("WiFi networks:"), FONT_SIZE_2, 0x4FA4U, 0x0U, 0xFFE0);

void initScreenAndTouch()
{
  // Initialise TFT screen
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(FONT_SIZE_2);

  //Initialise TFT Touch
  uint16_t calData[5] = { 242, 3583, 325, 3617, 4 };
  tft.setTouch(calData);

  Serial.println("Touch Initialised");
  tft.println("Touch Initialised");
}

void initWiFi()
{
  //Initialise ESP32 WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println("WiFi Initialised");
  tft.println("WiFi Initialised");
}

void initBME680()
{
  //Initialize BME680 detector
  Serial.print("BME680 test: ");
  tft.print("BME680 test: ");
  if (!bme.begin()) 
  {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    tft.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }
  Serial.println("Passed");
  tft.println("Passed");

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  Serial.println("BME680 values set");
  tft.println("BME680 values set");
}

void setup() 
{
  // Use serial port
  Serial.begin(115200);

  initScreenAndTouch();
  delay(100);

  initWiFi();
  delay(100);

  initBME680();
  delay(100);

  Serial.println("Software start in:");
  tft.println("Software start in:");
  for (size_t i = 5U; i > 0; i--)
  {
      Serial.println(i);
      tft.println(i);
    delay(1000);
  }
  
  //Fill screen with BG image
  tft.setSwapBytes(true); 
  tft.pushImage(0, 0, MAX_IMAGE_WIDTH, MAX_IMAGE_HIGHT, (uint16_t *)Horo_image.pixel_data);
}

  bool pressed;
  uint16_t x,y;
  int n;
void loop(void) 
{
  /* Serial.println("scan start");
  // WiFi.scanNetworks will return the number of networks found
  n = WiFi.scanNetworks();
  Serial.println("scan done");

  switch (n)
  {
    case 0:
      Serial.println("no networks found");
    break;
    
    default:
      Serial.print(n);
      Serial.println(" networks found");
      tft.setCursor(0,25);
      for (int i = 0; i < n; ++i) 
      {
        // Print SSID and RSSI for each network found
        tft.print(i + 1);
        tft.print(": ");
        tft.print(WiFi.SSID(i));
        tft.print(" (");
        tft.print(WiFi.RSSI(i));
        tft.print(")");
        tft.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      }
      
    break;
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(5000); */

  // Tell BME680 to begin measurement.
  unsigned long endTime = bme.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  Serial.print(F("Reading started at "));
  Serial.print(millis());
  Serial.print(F(" and will finish at "));
  Serial.println(endTime);

  Serial.println(F("You can do other work during BME680 measurement."));
  delay(50); // This represents parallel work.
  // There's no need to delay() until millis() >= endTime: bme.endReading()
  // takes care of that. It's okay for parallel work to take longer than
  // BME680's measurement time.

  // Obtain measurement results from BME680. Note that this operation isn't
  // instantaneous even if milli() >= endTime due to I2C/SPI latency.
  if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }
  Serial.print(F("Reading completed at "));
  Serial.println(millis());

  Serial.print(F("Temperature = "));
  Serial.print(bme.temperature);
  Serial.println(F(" *C"));

  Serial.print(F("Pressure = "));
  Serial.print(bme.pressure / 100.0);
  Serial.println(F(" hPa"));

  Serial.print(F("Humidity = "));
  Serial.print(bme.humidity);
  Serial.println(F(" %"));

  Serial.print(F("Gas = "));
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(F(" KOhms"));

  Serial.print(F("Approx. Altitude = "));
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(F(" m"));

  Serial.println();
  delay(2000);

  // Pressed will be set true is there is a valid touch on the screen
  pressed = tft.getTouch(&x, &y);

  // Draw a white spot at the detected coordinates
  if (pressed) 
  {
    Serial.print("x,y = ");
    Serial.print(x);
    Serial.print(",");
    Serial.println(y);
  }
}

// Code to run a screen calibration, not needed when calibration values set in setup()
/*
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
*/