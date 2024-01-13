#include <SPI.h>
#include <WiFi.h>
#include <SimpleFTPServer.h>

#include "SD.h"
#include "Horo.h"     //Default background file
#include "SpritedText.h"
#include "SensorBME680.h"
#include "SdBmpReader.h"

//SET_LOOP_TASK_STACK_SIZE( 156*1024 );

//------------START OF GLOBALS-----------------

//Multi threading globals 
TaskHandle_t xHandleBME680              = NULL;
TaskHandle_t xHandleFTP                 = NULL;
SemaphoreHandle_t xMutexBME680CStrings  = NULL;

//Screen globals
TFT_eSPI tft  = TFT_eSPI();

//Text on display globals
char aTemperatureCString[]  = "99.99 C";
char aHumidityCString[]     = "100.00%";
char aPressureCString[]     = "1000hPa";

SpritedText TextTemperature(&tft, MyCoordinates{5,170}, strlen(aTemperatureCString),  FONT_SIZE_1, 0x4FA4U, 0x0U, 0xFFF0U);
SpritedText TextHumidity(   &tft, MyCoordinates{5,220}, strlen(aHumidityCString),     FONT_SIZE_1, 0x4FA4U, 0x0U, 0xFFF0U);
SpritedText TextPressure(   &tft, MyCoordinates{5,270}, strlen(aPressureCString),     FONT_SIZE_1, 0x4FA4U, 0x0U, 0xFFF0U);

char aIAQCString[] = "999";

IAQText TextIAQ(&tft, MyCoordinates{0,0}, FONT_SIZE_1, 0x4FA4U, 0x0U, 0xFFF0);

//FTP globals
FtpServer ftpSrv;

//-------------END OF GLOBALS-----------------

void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace)
{
	Serial.print(">>>>>>>>>>>>>>> _callback " );
	Serial.print(ftpOperation);
	/* FTP_CONNECT,
	 * FTP_DISCONNECT,
	 * FTP_FREE_SPACE_CHANGE
	 */
	Serial.print(" ");
	Serial.print(freeSpace);
	Serial.print(" ");
	Serial.println(totalSpace);

	// freeSpace : totalSpace = x : 360

	if (ftpOperation == FTP_CONNECT) Serial.println(F("CONNECTED"));
	if (ftpOperation == FTP_DISCONNECT) Serial.println(F("DISCONNECTED"));
};

void _transferCallback(FtpTransferOperation ftpOperation, const char* name, unsigned int transferredSize)
{
        //Serial.printf("_transferCallback %d %s %u\n", ftpOperation, name, transferredSize);
        /* FTP_UPLOAD_START = 0,
        * FTP_UPLOAD = 1,
        *
        * FTP_DOWNLOAD_START = 2,
        * FTP_DOWNLOAD = 3,
        *
        * FTP_TRANSFER_STOP = 4,
        * FTP_DOWNLOAD_STOP = 4,
        * FTP_UPLOAD_STOP = 4,
        *
        * FTP_TRANSFER_ERROR = 5,
        * FTP_DOWNLOAD_ERROR = 5,
        * FTP_UPLOAD_ERROR = 5
        */

};

void taskBME680(void * pvParameters)
{
  while(true) 
  {
    if (SensorBME680::run()) 
    { // If new data is available
      if( xSemaphoreTake( xMutexBME680CStrings, portMAX_DELAY ) == pdTRUE )
      {
        snprintf(aTemperatureCString,  8U,  "%.1fC",    SensorBME680::getTemperature());
        snprintf(aHumidityCString,     8U,  "%.1f%%",   SensorBME680::getHumidity() + (float)HUMIDITY_OFFSET);
        snprintf(aPressureCString,     8U,  "%.0fhPa",  SensorBME680::getPressure() / 100.0f + (float)PRESSURE_OFFSET);
        snprintf(aIAQCString,          4U,  "%.0f",     SensorBME680::getIAQ());
        xSemaphoreGive( xMutexBME680CStrings );
      }

      Serial.print("Temperature = ");
      Serial.print(aTemperatureCString);
      Serial.print(" Humidity = ");
      Serial.print(aHumidityCString);
      Serial.print(" Pressure = ");
      Serial.print(aPressureCString);

      Serial.print(" IAQ = ");
      Serial.print(SensorBME680::getIAQ());
      Serial.print(" IAQ ACC = ");
      Serial.print(SensorBME680::getIAQaccuracy());
      Serial.println();
    } 

    Serial.print("BM680Stack: ");
    Serial.print(uxTaskGetStackHighWaterMark(xHandleBME680));
    Serial.println(" Bytes");

    delay(LOW_POWER_MODE_SENSOR_READ_PERIOD);
  }
}

void taskFTP(void * pvParameters)
{
  while(true)
  {
    ftpSrv.handleFTP();
    delay(5);
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

void reinitScreen()
{
  // Initialise TFT screen
  tft.init();
  tft.setTextSize(FONT_SIZE_2);
  tft.pushImage(0, 0, MAX_IMAGE_WIDTH, MAX_IMAGE_HIGHT, (uint16_t *)Horo_image.pixel_data);
}

void initSDMMC()
{
  pinMode(GPIO_NUM_2, INPUT_PULLUP);
  pinMode(GPIO_NUM_15, INPUT_PULLUP);
  pinMode(GPIO_NUM_14, INPUT_PULLUP);

    if(!SD_MMC.begin("/sdcard", ONE_BIT_MODE, true))
    {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD_MMC.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD_MMC card attached");
        return;
    }

    Serial.print("SD_MMC Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
    Serial.println("SD opened!");
}

void initWiFi()
{
  WiFi.begin(SSID, PASSWORD);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void initFTP()
{
    ftpSrv.setCallback(_callback);
    ftpSrv.setTransferCallback(_transferCallback);
    ftpSrv.begin("","");    //username, password for ftp.   (default 21, 50009 for PASV)
}

void setup() 
{
  // Use serial port
  Serial.begin(115200);

  initScreenAndTouch();
  delay(100);

  SensorBME680::init(&tft);
  delay(100);

  initSDMMC();
  delay(100);

  initWiFi();
  delay(100);

  initFTP();
  delay(100);

  Serial.println("Software start in:");
  tft.println("Software start in:");
  for (size_t i = 5U; i > 0; i--)
  {
      Serial.println(i);
      tft.println(i);
    delay(1000);
  }
  
  //BMP temp("/sdcard/240x320/Fumo.bmp");
  //Fill screen with BG image
  tft.setSwapBytes(true); 
  tft.pushImage(0, 0, MAX_IMAGE_WIDTH, MAX_IMAGE_HIGHT, (uint16_t *)Horo_image.pixel_data);

  xMutexBME680CStrings = xSemaphoreCreateMutex();
  xTaskCreate(taskBME680,       // Function that implements the task. 
              "BME680",         // Text name for the task. 
              STACK_SIZE_BME680,// Stack size in words, not bytes. 
              nullptr,          // Parameter passed into the task. 
              10U,               // Priority at which the task is created. 
              &xHandleBME680 ); // Used to pass out the created task's handle. 

  xTaskCreate(taskFTP,
              "FTP",
              STACK_SIZE_FTP,
              nullptr,
              3u,
              &xHandleFTP);
}

  bool pressed = false;
  uint16_t x,y;

void loop(void) 
{
  pressed = tft.getTouch(&x, &y);
  if (pressed) 
  {
    Serial.print("x,y = ");
    Serial.print(x);
    Serial.print(",");
    Serial.println(y);
    reinitScreen();
  }

    if( xSemaphoreTake( xMutexBME680CStrings, portMAX_DELAY ) == pdTRUE )
    {
      TextTemperature.setCString(aTemperatureCString);
      TextHumidity.setCString(aHumidityCString);
      TextPressure.setCString(aPressureCString);
      TextIAQ.setCString(aIAQCString);
      
      xSemaphoreGive( xMutexBME680CStrings );
    }

    TextTemperature.printText();
    TextHumidity.printText();
    TextPressure.printText();
    TextIAQ.printText();

  delay(33);
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