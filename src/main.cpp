#include <SPI.h>
#include <EEPROM.h>

#include "bsec.h"

#include "Horo.h"         //Background file
#include "SpritedText.h"

//------------START OF GLOBALS-----------------

//Multi threading globals 
TaskHandle_t xHandleBME680              = NULL;
SemaphoreHandle_t xMutexBME680CStrings  = NULL;

//Screen globals
TFT_eSPI tft  = TFT_eSPI();

//BME680 globals
Bsec iaqSensor;
uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
uint16_t stateUpdateCounter = 0;

const uint8_t bsec_config_iaq[] = { 
#include "config/generic_33v_3s_28d/bsec_iaq.txt" 
};

//Text on display globals
char aTemperatureCString[]  = "99.99 C";
char aHumidityCString[]     = "100.00%";
char aPressureCString[]     = "1000hPa";

SpritedText TextTemperature(&tft, MyCoordinates{5,170}, strlen(aTemperatureCString),  FONT_SIZE_1, 0x4FA4U, 0x0U, 0xFFE0);
SpritedText TextHumidity(   &tft, MyCoordinates{5,220}, strlen(aHumidityCString),     FONT_SIZE_1, 0x4FA4U, 0x0U, 0xFFE0);
SpritedText TextPressure(   &tft, MyCoordinates{5,270}, strlen(aPressureCString),     FONT_SIZE_1, 0x4FA4U, 0x0U, 0xFFE0);

//-------------END OF GLOBALS-----------------

// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.bsecStatus != BSEC_OK) 
  {
    if (iaqSensor.bsecStatus < BSEC_OK) 
    {
      Serial.print("BSEC error code : ");
      Serial.println(iaqSensor.bsecStatus);
      for (;;); /* Halt in case of failure */
    } 
    else 
    {
      Serial.print("BSEC warning code : ");
      Serial.println(iaqSensor.bsecStatus);
    }
  }

  if (iaqSensor.bme68xStatus != BME68X_OK) 
  {
    if (iaqSensor.bme68xStatus < BME68X_OK) 
    {
      Serial.print("BME68X error code : ");
      Serial.println(iaqSensor.bme68xStatus);
      for (;;); /* Halt in case of failure */
    } 
    else 
    {
      Serial.print("BME68X warning code : ");
      Serial.println(iaqSensor.bme68xStatus);
    }
  }
}

void loadState(void)
{
  if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE) 
  {
    // Existing state in EEPROM
    Serial.println("Reading state from EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++) 
    {
      bsecState[i] = EEPROM.read(i + 1);
      Serial.println(bsecState[i], HEX);
    }

    iaqSensor.setState(bsecState);
    checkIaqSensorStatus();
  } 
  else 
  {
    // Erase the EEPROM with zeroes
    Serial.println("Erasing EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
    {
      EEPROM.write(i, 0);
    }

    EEPROM.commit();
  }
}

void updateState(void)
{
  bool update = false;
  if (stateUpdateCounter == 0) 
  {
    /* First state update when IAQ accuracy is >= 3 */
    if (iaqSensor.iaqAccuracy >= 3) 
    {
      update = true;
      stateUpdateCounter++;
    }
  } 
  else 
  {
    /* Update every STATE_SAVE_PERIOD minutes */
    if ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis()) 
    {
      update = true;
      stateUpdateCounter++;
    }
  }

  if (update) 
  {
    iaqSensor.getState(bsecState);
    checkIaqSensorStatus();

    Serial.println("Writing state to EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE ; i++) 
    {
      EEPROM.write(i + 1, bsecState[i]);
      Serial.println(bsecState[i], HEX);
    }

    EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
    EEPROM.commit();
  }
}

void taskBME680(void * pvParameters)
{
  while(true) 
  {
    if (iaqSensor.run()) 
    { // If new data is available
      if( xSemaphoreTake( xMutexBME680CStrings, portMAX_DELAY ) == pdTRUE )
      {
        /* We were able to obtain the semaphore and can now access the
        shared resource. */
        sprintf(aTemperatureCString, "%.1fC", iaqSensor.temperature);
        /* We have finished accessing the shared resource.  Release the
        semaphore. */
        xSemaphoreGive( xMutexBME680CStrings );
      }

      Serial.print("Temperature = ");
      Serial.print(aTemperatureCString);

      if( xSemaphoreTake( xMutexBME680CStrings, portMAX_DELAY ) == pdTRUE )
      {
        /* We were able to obtain the semaphore and can now access the
        shared resource. */
        sprintf(aHumidityCString, "%.1f%%", iaqSensor.humidity + (float)HUMIDITY_OFFSET);
        /* We have finished accessing the shared resource.  Release the
        semaphore. */
        xSemaphoreGive( xMutexBME680CStrings );
      }

      Serial.print("Humidity = ");
      Serial.print(aHumidityCString);

      if( xSemaphoreTake( xMutexBME680CStrings, portMAX_DELAY ) == pdTRUE )
      {
        /* We were able to obtain the semaphore and can now access the
        shared resource. */
        sprintf(aPressureCString, "%.0fhPa", iaqSensor.pressure / 100.0f + (float)PRESSURE_OFFSET);
        /* We have finished accessing the shared resource.  Release the
        semaphore. */
        xSemaphoreGive( xMutexBME680CStrings );
      }

      Serial.print("Pressure = ");
      Serial.print(aPressureCString);
      Serial.println();

      updateState();
    } 
    else 
    {
      checkIaqSensorStatus();
    }

    Serial.print("BM680Stack:");
    Serial.print(uxTaskGetStackHighWaterMark(xHandleBME680));
    Serial.println("Bytes");
    delay(LOW_POWER_MODE_DETECTOR_READ_PERIOD);
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
  iaqSensor.begin(BME68X_I2C_ADDR_HIGH, Wire);
  String LibraryVersion = "BSEC library version" + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(LibraryVersion);
  tft.println(LibraryVersion);
  checkIaqSensorStatus();

  iaqSensor.setTemperatureOffset(1.0f);
  iaqSensor.setConfig(bsec_config_iaq);
  checkIaqSensorStatus();

  loadState();
  Serial.println("EEPROM config loaded");
  tft.println("EEPROM config loaded");

  bsec_virtual_sensor_t sensorList[13] = {
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_STABILIZATION_STATUS,
    BSEC_OUTPUT_RUN_IN_STATUS,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    BSEC_OUTPUT_GAS_PERCENTAGE
  };

  iaqSensor.updateSubscription(sensorList, 13, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();

  Serial.println("BME680 Initialised");
  tft.println("BME680 Initialised");
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