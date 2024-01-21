#include <stdio.h>
#include <byteswap.h>

#include <WiFi.h>
#include <SimpleFTPServer.h>
#include <DFRobot_SD3031.h>

#include "WiFiSignal/WiFiSignal.h"
#include "Sprite/SpriteSubject.h"
#include "SpritedText/SpritedText.h"
#include "SensorBME680.h"
#include "PNGDecoder.h"

//SET_LOOP_TASK_STACK_SIZE( 64*1024 ); only for testing

//------------START OF GLOBALS-----------------

//Multi threading globals 
TaskHandle_t xHandleBME680  = NULL;
TaskHandle_t xHandleRTC     = NULL;
TaskHandle_t xHandleFTP     = NULL;

SemaphoreHandle_t xMutexSpritedTextCStringAccess    = NULL;

//Screen globals
TFT_eSPI tft  = TFT_eSPI();

//RTC globals
DFRobot_SD3031 rtc;

//Text on display globals

#pragma region MainScreen
SpriteSubject subjectMainScreen;

SpritedText TextTemperature(&tft, &xMutexSpritedTextCStringAccess, Coordinates{5,205},   strlen("99.99C"),     FONT_SIZE_3, 0xFFFFU, 0x0U, 0x0U);
SpritedText TextHumidity(   &tft, &xMutexSpritedTextCStringAccess, Coordinates{5,240},   strlen("100.00%"),    FONT_SIZE_3, 0xFFFFU, 0x0U, 0x0U);
SpritedText TextPressure(   &tft, &xMutexSpritedTextCStringAccess, Coordinates{5,275},   strlen("1000hPa"),    FONT_SIZE_3, 0xFFFFU, 0x0U, 0x0U);
SpritedText TextDate(       &tft, &xMutexSpritedTextCStringAccess, Coordinates{59,5},    strlen("9999/99/99"), FONT_SIZE_3, 0xF800,  0x0U, 0x0U);
SpritedText TextTime(       &tft, &xMutexSpritedTextCStringAccess, Coordinates{73,35},   strlen("99:99:99"),   FONT_SIZE_3, 0xF800,  0x0U, 0x0U);

WiFiSignal SpriteWiFiSignal(&tft, Coordinates{200 ,280});

IAQText TextIAQ(&tft, &xMutexSpritedTextCStringAccess, Coordinates{5,5}, strlen("999"), FONT_SIZE_3, 0x0U, 0xFFF0U);

#pragma endregion MainScreen

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
      Serial.printf("Task FTP: %u Bytes free on stack\n", uxTaskGetStackHighWaterMark(xHandleFTP));
};

void taskBME680(void * pvParameters)
{
  sTimeData_t sTime;

  char aDataOutCString[100];
  char aFileName[50];

  FILE* dataOut;

  float temperature, humidity, pressure, IAQ;
  uint8_t IAQACC;

  char* pCharTemperature, *pCharHumidity, *pCharPressure, *pCharIAQ;

  pCharTemperature  = TextTemperature.getCharArrayPtr();
  pCharHumidity     = TextHumidity.getCharArrayPtr();
  pCharPressure     = TextPressure.getCharArrayPtr();
  pCharIAQ          = TextIAQ.getCharArrayPtr();
  
  for (size_t i = 0; i < 50; i++)
  {
    sTime = rtc.getRTCTime();

    if(sTime.year == 2165)//workaround for noise on i2c genereted from internal power supply 
    {
      delay(10);
      continue;
    }
    break;
  }
  
  sprintf(aFileName,"/sdcard/SensorData/%04u_%02u_%02u_%02u_%02u_%02u.csv", sTime.year, sTime.month, sTime.day, sTime.hour, sTime.minute, sTime.second);
  
  Serial.println(aFileName);
  dataOut = fopen(aFileName, "a");
  fprintf(dataOut, "Time,Temperature,Humidity,Pressure,IAQ");

  fclose(dataOut);

  while(true) 
  {
    if (SensorBME680::run()) 
    { // If new data is available
      temperature = SensorBME680::getTemperature();
      humidity    = SensorBME680::getHumidity() + (float)HUMIDITY_OFFSET;
      pressure    = SensorBME680::getPressure() / 100.0f + (float)PRESSURE_OFFSET;
      IAQ         = SensorBME680::getIAQ();
      IAQACC      = SensorBME680::getIAQaccuracy();

      if( xSemaphoreTake( xMutexSpritedTextCStringAccess, portMAX_DELAY ) == pdTRUE )
      {
        snprintf(pCharTemperature,  8U,  "%.1fC",    temperature);
        snprintf(pCharHumidity,     8U,  "%.1f%%",   humidity);
        snprintf(pCharPressure,     8U,  "%.0fhPa",  pressure);
        snprintf(pCharIAQ,          4U,  "%.0f",     IAQ);

        xSemaphoreGive( xMutexSpritedTextCStringAccess );
      }

      sprintf(aDataOutCString,"\n%u,%.1f,%.1f,%.1f,%.1f", millis(), temperature, humidity, pressure, IAQ);

      dataOut = fopen(aFileName, "a");
      fprintf(dataOut, aDataOutCString);
      fclose(dataOut);

      Serial.printf("Temperature = %.1fC Humidity = %.1f%% Pressure = %.0fhPa IAQ = %.0f IAQ ACC = %u\n", temperature, humidity, pressure, IAQ, IAQACC);
    } 

    Serial.printf("Task BM680: %u Bytes free on stack\n", uxTaskGetStackHighWaterMark(xHandleBME680));

    delay(LOW_POWER_MODE_SENSOR_READ_PERIOD);
  }
}

void taskRTC(void * pvParameters)
{    
  sTimeData_t sTime;

  char* pCharDate = TextDate.getCharArrayPtr();
  char* pCharTime = TextTime.getCharArrayPtr();

  while(true)
  {
    sTime = rtc.getRTCTime();

    if(sTime.year == 2165)//workaround for noise on i2c genereted from internal power supply 
    {
      delay(10);
      continue;
    }

    if( xSemaphoreTake( xMutexSpritedTextCStringAccess, portMAX_DELAY ) == pdTRUE )
    {
      snprintf(pCharDate,  11U,  "%04u/%02u/%02u",  sTime.year, sTime.month, sTime.day);
      snprintf(pCharTime,  9U,  "%02u:%02u:%02u",   sTime.hour, sTime.minute, sTime.second);

      xSemaphoreGive( xMutexSpritedTextCStringAccess);
    }

    Serial.print(sTime.year, DEC);//year
    Serial.print('/');
    Serial.print(sTime.month, DEC);//month
    Serial.print('/');
    Serial.print(sTime.day, DEC);//day
    Serial.print(" (");
    Serial.print(sTime.week);//week
    Serial.print(") ");
    Serial.print(sTime.hour, DEC);//hour
    Serial.print(':');
    Serial.print(sTime.minute, DEC);//minute
    Serial.print(':');
    Serial.print(sTime.second, DEC);//second
    Serial.println(' ');

    delay(ONE_SECOND_TICK);
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

void initRTC()
{
  Serial.println("RTC starting ");
  tft.println("RTC starting ");
  for (int i = 0;i<20 && rtc.begin() != 0; ++i) 
  {
    Serial.print(".");
    tft.print(".");
    delay(500);
  }

  if(rtc.begin() == 0)
  {
    Serial.println("RTC started ");
    tft.println("RTC started ");
  }
  else
  {
    Serial.println("RTC failed ");
    tft.println("RTC failed ");
  }
  //rtc.setHourSystem(rtc.e24hours);//Set display format only needed for first time after new battery is added
  //rtc.setTime(2024,1,18,16,55,0);//Initialize time
}

void initI2C()
{
  Wire.setPins(I2C_SDA, I2C_SCL);

  SensorBME680::init(&tft);
  delay(100);

  initRTC();
  delay(100);
}

void reinitScreen()
{
  // Initialise TFT screen
  tft.init();
  tft.setTextSize(FONT_SIZE_2);

  switch (MENU_INDEX_VAR)
  {
    case MENU_MAIN:
    {
        PNGDecoder::setBackground(&tft, "/Backgrounds/Default.png");

        subjectMainScreen.NotifysetSpriteBackground(&tft);
        subjectMainScreen.NotifyForcePrintSprite();
    }break;
    
    default:
      break;
  }

}

void initSDMMC()
{

  SD_MMC.setPins(GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_2);
    if(!SD_MMC.begin("/sdcard", ONE_BIT_MODE, true))
    {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD_MMC.cardType();

    if(cardType == CARD_NONE)
    {
        Serial.println("No SD_MMC card attached");
        return;
    }

    Serial.print("SD_MMC Card Type: ");
    if(cardType == CARD_MMC)
    {
        Serial.println("MMC");
    } 
    else if(cardType == CARD_SD)
    {
        Serial.println("SDSC");
    } 
    else if(cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    } 
    else 
    {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
    Serial.println("SD opened!");
}

void initWiFi()
{
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("Connecting to WiFi\n");
  tft.printf("Connecting to WiFi\n");

  // Wait for connection
  for (int i = 0;i<20 && WiFi.status() != WL_CONNECTED; ++i) 
  {
    delay(500);
    Serial.print(".");
    tft.print(".");
  }

  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.printf("\nConnected to %s\nIP address:\n%s\n", SSID, WiFi.localIP().toString().c_str());
    tft.printf("\nConnected to %s\nIP address:\n%s\n", SSID, WiFi.localIP().toString().c_str());
  }
  else
  {
    Serial.printf("WiFi disconected code:%d\n", WiFi.status());
    tft.printf("WiFi disconected code:%d\n", WiFi.status());
  }
  
}

void initFTP()
{
  if(WiFi.status() == WL_CONNECTED)
  {
    ftpSrv.setCallback(_callback);
    ftpSrv.setTransferCallback(_transferCallback);
    ftpSrv.begin("","");    //username, password for ftp.   (default 21, 50009 for PASV)
  }
  else
  {
    Serial.printf("FTP not initialized\n");
    tft.printf("FTP not initialized\n");
  }
}

void setup() 
{
  // Use serial port
  Serial.begin(115200);

  initScreenAndTouch();
  delay(100);

  initI2C();
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
  
  PNGDecoder::setBackground(&tft, "/Backgrounds/Default.png");

  subjectMainScreen.Attach(&TextHumidity);
  subjectMainScreen.Attach(&TextTemperature);
  subjectMainScreen.Attach(&TextPressure);
  subjectMainScreen.Attach(&TextDate);
  subjectMainScreen.Attach(&TextIAQ);
  subjectMainScreen.Attach(&TextTime);
  subjectMainScreen.Attach(&SpriteWiFiSignal);

  subjectMainScreen.NotifyCreateSprite();
  subjectMainScreen.NotifysetSpriteBackground(&tft);
  subjectMainScreen.NotifyPrintSprite();

  xMutexSpritedTextCStringAccess = xSemaphoreCreateMutex();
  xTaskCreate(taskBME680,       // Function that implements the task. 
              "BME680",         // Text name for the task. 
              STACK_SIZE_BME680,// Stack size in words, not bytes. 
              nullptr,          // Parameter passed into the task. 
              10U,               // Priority at which the task is created. 
              &xHandleBME680 ); // Used to pass out the created task's handle. 

  xTaskCreate(taskRTC,
              "RTC",
              STACK_SIZE_RTC,
              nullptr,
              10u,
              &xHandleRTC);

  xTaskCreate(taskFTP,
              "FTP",
              STACK_SIZE_FTP,
              nullptr,
              3u,
              &xHandleFTP);
}

  bool pressed  = false;
  bool lifted   = true;
  uint16_t x,y;

void loop(void) 
{
  pressed = tft.getTouch(&x, &y);
  if (pressed) 
  {
    Serial.printf("x,y = %u,%u\n", x, y);
    if(lifted)
    {
      switch (MENU_INDEX_VAR)
      {
        case MENU_MAIN:
        {
          if(SpriteWiFiSignal.BoundsOf(x, y))
          {
            MENU_INDEX_VAR = MENU_WIFI;
          }
        }break;

        case MENU_WIFI:
        {
          MENU_INDEX_VAR = MENU_MAIN;

          PNGDecoder::setBackground(&tft, "/Backgrounds/Default.png");

          subjectMainScreen.NotifysetSpriteBackground(&tft);
          subjectMainScreen.NotifyForcePrintSprite();
        }break;

        default:
          break;
      }
      lifted = false;
    }
  }
  else
  {
    lifted = true;
  }

  switch (MENU_INDEX_VAR)
  {
    case MENU_MAIN:
    {
      subjectMainScreen.NotifyPrintSprite();
    }break;

    case MENU_WIFI:
    {
      tft.fillScreen(TFT_BLACK);
      tft.printf("Connected: %s\n Signal:%d", WiFi.SSID(), WiFi.RSSI());
      tft.setCursor(5,5);
    }break;

    default:
      break;
  }

  delay(100);
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

sTimeData_t sTime;
  sTime = rtc.getRTCTime();

  Serial.printf("%u-%u-%u\n", sTime.year, sTime.month, sTime.day);

  Serial.print(sTime.year, DEC);//year
  Serial.print('/');
  Serial.print(sTime.month, DEC);//month
  Serial.print('/');
  Serial.print(sTime.day, DEC);//day
  Serial.print(" (");
  Serial.print(sTime.week);//week
  Serial.print(") ");
  Serial.print(sTime.hour, DEC);//hour
  Serial.print(':');
  Serial.print(sTime.minute, DEC);//minute
  Serial.print(':');
  Serial.print(sTime.second, DEC);//second
  Serial.println(' ');

*/