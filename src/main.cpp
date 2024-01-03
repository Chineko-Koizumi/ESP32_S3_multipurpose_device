#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#include "Horo.h"         //Background file
#include "SpritedText.h"

TaskHandle_t xHandleBME680              = NULL;
SemaphoreHandle_t xMutexBME680CStrings  = NULL;

TFT_eSPI tft  = TFT_eSPI();

Adafruit_BME680 bme; // I2C

char aTemperatureCString[]  = "99.99 C";
char aHumidityCString[]     = "100.00%";
char aPressureCString[]     = "1000hPa";

SpritedText TextTemperature(&tft, MyCoordinates{5,150}, strlen(aTemperatureCString),  FONT_SIZE_4, 0x4FA4U, 0x0U, 0xFFE0);
SpritedText TextHumidity(   &tft, MyCoordinates{5,190}, strlen(aHumidityCString),     FONT_SIZE_4, 0x4FA4U, 0x0U, 0xFFE0);
SpritedText TextPressure(   &tft, MyCoordinates{5,230}, strlen(aPressureCString),     FONT_SIZE_4, 0x4FA4U, 0x0U, 0xFFE0);

void taskBME680(void * pvParameters)
{
  unsigned long endTime, startTime;

  while(true) 
  {
    startTime = millis();
    endTime   = bme.beginReading(); // Tell BME680 to begin measurement.

    if (endTime == 0) 
    {
      Serial.println("Failed to begin reading :(");
    }
    else
    {
      delay(endTime - startTime + 30U); 
      // This represents parallel work.
      // There's no need to delay() until millis() >= endTime: bme.endReading()
      // takes care of that. It's okay for parallel work to take longer than
      // BME680's measurement time.

      // Obtain measurement results from BME680. Note that this operation isn't
      // instantaneous even if milli() >= endTime due to I2C/SPI latency.
      if (!bme.endReading()) 
      {
        Serial.println("Failed to complete reading :(");
      }
      else
      {
        if( xSemaphoreTake( xMutexBME680CStrings, portMAX_DELAY ) == pdTRUE )
        {
          /* We were able to obtain the semaphore and can now access the
          shared resource. */
          sprintf(aTemperatureCString, "%.1f C", bme.temperature);
          /* We have finished accessing the shared resource.  Release the
          semaphore. */
          xSemaphoreGive( xMutexBME680CStrings );
        }

        Serial.print("Temperature = ");
        Serial.print(bme.temperature);
        Serial.println(" *C");

        if( xSemaphoreTake( xMutexBME680CStrings, portMAX_DELAY ) == pdTRUE )
        {
          /* We were able to obtain the semaphore and can now access the
          shared resource. */
          sprintf(aHumidityCString, "%.1f%%", bme.humidity);
          /* We have finished accessing the shared resource.  Release the
          semaphore. */
          xSemaphoreGive( xMutexBME680CStrings );
        }

        Serial.print("Humidity = ");
        Serial.print(bme.humidity);
        Serial.println(" %");

        if( xSemaphoreTake( xMutexBME680CStrings, portMAX_DELAY ) == pdTRUE )
        {
          /* We were able to obtain the semaphore and can now access the
          shared resource. */
          sprintf(aPressureCString, "%luhPa", bme.pressure / 100);
          /* We have finished accessing the shared resource.  Release the
          semaphore. */
          xSemaphoreGive( xMutexBME680CStrings );
        }

        Serial.print("Pressure = ");
        Serial.print(bme.pressure / 100.0);
        Serial.println(" hPa");

        Serial.print("Gas = ");
        Serial.print(bme.gas_resistance / 1000.0);
        Serial.println(" KOhms");

        Serial.println();
      }
    }
    Serial.print("Stack:");
    Serial.println(uxTaskGetStackHighWaterMark(xHandleBME680));
    delay(2000);
  }
}

void initScreenAndTouch()
{
  // Initialise TFT screen
  tft.init();
  tft.setTextSize(FONT_SIZE_2);
  tft.fillScreen(TFT_BLACK);

  //Initialise TFT Touch
  uint16_t calData[5] = { 242, 3583, 325, 3617, 4 };
  tft.setTouch(calData);

  Serial.println("Touch Initialised");
  tft.println("Touch Initialised");
}

/* void initWiFi()
{
  //Initialise ESP32 WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println("WiFi Initialised");
  tft.println("WiFi Initialised");
}
*/

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

  //initWiFi();
  //delay(100);

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

  xMutexBME680CStrings = xSemaphoreCreateMutex();

  xTaskCreate(taskBME680,       // Function that implements the task. 
              "BME680",         // Text name for the task. 
              STACK_SIZE_BME680,// Stack size in words, not bytes. 
              nullptr,          // Parameter passed into the task. 
              3U,               // Priority at which the task is created. 
              &xHandleBME680 ); // Used to pass out the created task's handle. 
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

    if( xSemaphoreTake( xMutexBME680CStrings, portMAX_DELAY ) == pdTRUE )
    {
      /* We were able to obtain the semaphore and can now access the
      shared resource. */
      TextTemperature.setCString(aTemperatureCString);
      /* We have finished accessing the shared resource.  Release the
      semaphore. */
      xSemaphoreGive( xMutexBME680CStrings );
    }

    if( xSemaphoreTake( xMutexBME680CStrings, portMAX_DELAY ) == pdTRUE )
    {
      /* We were able to obtain the semaphore and can now access the
      shared resource. */
      TextHumidity.setCString(aHumidityCString);
      /* We have finished accessing the shared resource.  Release the
      semaphore. */
      xSemaphoreGive( xMutexBME680CStrings );
    }

    if( xSemaphoreTake( xMutexBME680CStrings, portMAX_DELAY ) == pdTRUE )
    {
      /* We were able to obtain the semaphore and can now access the
      shared resource. */
      TextPressure.setCString(aPressureCString);
      /* We have finished accessing the shared resource.  Release the
      semaphore. */
      xSemaphoreGive( xMutexBME680CStrings );
    }

    TextTemperature.printText();
    TextHumidity.printText();
    TextPressure.printText();

  delay(32);
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