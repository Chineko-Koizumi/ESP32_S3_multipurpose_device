#include <stdio.h>

#include <WiFi.h>
#include <SimpleFTPServer.h>
#include <DFRobot_SD3031.h>

#include "DisplayLabel/DisplayLabel.h"
#include "Sprites/DefaultBackground/Horo.h"
#include "SensorBME680.h"

SET_LOOP_TASK_STACK_SIZE( 64*1024 );

//------------START OF GLOBALS-----------------

//Multi threading globals 
TaskHandle_t xHandleI2C     = NULL;
TaskHandle_t xHandleFTP     = NULL;

SemaphoreHandle_t xMutexLabelUpdate    = NULL;

//RTC globals
DFRobot_SD3031 rtc;

//Text on display globals
#pragma region MainScreen

uint8_t draw_buf[DRAW_BUF_SIZE];
lv_display_t * display    = nullptr;
lv_indev_t  *inputDevice  = nullptr;
lv_obj_t    *background   = nullptr;

TFT_eSPI * tft = nullptr;

lv_style_t defaultLabelStyle;

DisplayLabel *LabelTemperature  = nullptr;
DisplayLabel *LabelPressure     = nullptr;
DisplayLabel *LabelHumidity     = nullptr;
DisplayLabel *LabelTime         = nullptr;
DisplayLabel *LabelDate         = nullptr;

DisplayLabelIAQ  *LabelIAQ      = nullptr;

#pragma endregion MainScreen

//FTP globals
FtpServer ftpSrv;

//-------------END OF GLOBALS-----------------

void touchpad_read( lv_indev_t * inputDevice, lv_indev_data_t * data )
{
    static uint16_t x, y;

    if(tft->getTouch( &x, &y )) 
    {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
        
        Serial.printf("%U, %U\n", x, y);
    } 
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

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

void taskI2C(void * pvParameters)
{    
  FILE* dataOut;

  sTimeData_t sTime;
  uint32_t currentSecond  = 0U;
  uint64_t startTime    = 0U;
  int64_t deltaTime = 0;

  char aDataOutCString[100];
  char aFileName[50];

  char aCharDate[20];
  char aCharTime[20];
  char aCharTemperature[20]; 
  char aCharHumidity[20]; 
  char aCharPressure[20];

  float temperature, humidity, pressure, IAQ;

  uint8_t IAQACC;
  /*
    IAQ Accuracy=0 could either mean:
    BSEC was just started, and the sensor is stabilizing (this lasts normally 5min in LP mode or 20min in ULP mode),
    there was a timing violation (i.e. BSEC was called too early or too late), which should be indicated by a warning/error flag by BSEC,

    IAQ Accuracy=1 means the background history of BSEC is uncertain. This typically means the gas sensor data was too stable for BSEC to clearly define its references,
   
   
    IAQ Accuracy=2 means BSEC found a new calibration data and is currently calibrating,

    IAQ Accuracy=3 means BSEC calibrated successfully.
  */

  for (size_t i = 0; i < 50; i++)
  {
    sTime = rtc.getRTCTime();

    if(sTime.year == 2165)//workaround for noise on i2c genereted from internal power supply 
    {
      delay(50);
      continue;
    }
    break;
  }
  
  sprintf(aFileName,"/sdcard/SensorData/%04u_%02u_%02u_%02u_%02u_%02u.csv", sTime.year, sTime.month, sTime.day, sTime.hour, sTime.minute, sTime.second);
  
  Serial.println(aFileName);
  dataOut = fopen(aFileName, "a");

  if(dataOut == NULL) 
  {
    Serial.printf("Error: %d (%s)\n", errno, strerror(errno));
  }
  else
  {
    fprintf(dataOut, "Time,Temperature,Humidity,Pressure,IAQ");
    fclose(dataOut);
  }

  while(true)
  {
    startTime = millis();

    sTime = rtc.getRTCTime();

    if(sTime.year == 2165)//workaround for noise on i2c genereted from internal power supply 
    {
      delay(10);
      continue;
    }
    snprintf(aCharDate,  11U,  "%04u/%02u/%02u",  sTime.year, sTime.month, sTime.day);
    snprintf(aCharTime,  9U,  "%02u:%02u:%02u",   sTime.hour, sTime.minute, sTime.second);

    if( xSemaphoreTake( xMutexLabelUpdate, portMAX_DELAY ) == pdTRUE )
    {
        LabelTime->SetLabelText(aCharTime);
      //LabelDate->SetLabelText(aCharDate);

      xSemaphoreGive( xMutexLabelUpdate);
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

    delay(10);
    if( (currentSecond % 3) == 0)
    {
      if (SensorBME680::run()) 
      { // If new data is available
        temperature = SensorBME680::getTemperature();
        humidity    = SensorBME680::getHumidity() + (float)HUMIDITY_OFFSET;
        pressure    = SensorBME680::getPressure() / 100.0f + (float)PRESSURE_OFFSET;
        IAQ         = SensorBME680::getIAQ();
        IAQACC      = SensorBME680::getIAQaccuracy();

        snprintf(aCharTemperature,  8U,  "%.1f°C",    temperature);
        snprintf(aCharHumidity,     8U,  "%.1f%%",   humidity);
        snprintf(aCharPressure,     8U,  "%.0fhPa",  pressure);
        
        if( xSemaphoreTake( xMutexLabelUpdate, portMAX_DELAY ) == pdTRUE )
        {
          LabelTemperature->SetLabelText(aCharTemperature);
          xSemaphoreGive( xMutexLabelUpdate);
        }

        if( xSemaphoreTake( xMutexLabelUpdate, portMAX_DELAY ) == pdTRUE )
        {
          LabelHumidity->SetLabelText(aCharHumidity);
          xSemaphoreGive( xMutexLabelUpdate);
        }

        if( xSemaphoreTake( xMutexLabelUpdate, portMAX_DELAY ) == pdTRUE )
        {
          LabelPressure->SetLabelText(aCharPressure);

          xSemaphoreGive( xMutexLabelUpdate);
        }

        if( xSemaphoreTake( xMutexLabelUpdate, portMAX_DELAY ) == pdTRUE )
        {
          LabelIAQ->SetIAQValue(IAQ);
          xSemaphoreGive( xMutexLabelUpdate);
        }

        sprintf(aDataOutCString,"\n%u,%.1f,%.1f,%.1f,%.1f", millis(), temperature, humidity, pressure, IAQ);

        dataOut = fopen(aFileName, "a");

        if(dataOut == NULL) 
        {
          Serial.printf("Error: %d (%s)\n", errno, strerror(errno));
        }
        else
        {
          fprintf(dataOut, aDataOutCString);
          fclose(dataOut);
        }

        Serial.printf("Temperature = %.1fC Humidity = %.1f%% Pressure = %.0fhPa IAQ = %.0f IAQ ACC = %u\n", temperature, humidity, pressure, IAQ, IAQACC);
      } 

      Serial.printf("Task RTC: %u Bytes free on stack\n", uxTaskGetStackHighWaterMark(xHandleI2C));

    }

    deltaTime = millis() - startTime;
    if(deltaTime < ONE_SECOND_TICK)
    {
      delay(ONE_SECOND_TICK - deltaTime);
    }
    ++currentSecond;
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

void initLVGL()
{
  lv_init();

    display = lv_tft_espi_create(MAX_IMAGE_WIDTH, MAX_IMAGE_HIGHT, draw_buf, DRAW_BUF_SIZE);
    lv_display_set_default(display);

    struct TFT_Init
    {
        TFT_eSPI * tft;
    };

    TFT_Init * temp = reinterpret_cast<TFT_Init *>(display->driver_data);
    tft = temp->tft;

    Serial.printf("Casted pointer: %X\n", tft);

    tft->init();
    tft->setTextSize(2U);
    tft->fillScreen(TFT_BLACK);

    //-------------------------------------------------------------------
    //-----------------------Default Style-------------------------------
    //-------------------------------------------------------------------
        lv_style_init(&defaultLabelStyle);
        lv_style_set_bg_color(&defaultLabelStyle, lv_color_hex(0x115588));
        lv_style_set_bg_opa(&defaultLabelStyle, LV_OPA_50);
        lv_style_set_border_width(&defaultLabelStyle, 2);
        lv_style_set_border_color(&defaultLabelStyle, lv_color_black());
        lv_style_set_text_font(&defaultLabelStyle, &lv_font_montserrat_32);
    //-------------------------------------------------------------------

    inputDevice = lv_indev_create();
    lv_indev_set_type(inputDevice, LV_INDEV_TYPE_POINTER);
    lv_indev_set_mode(inputDevice, LV_INDEV_MODE_TIMER);          //---+
    lv_indev_set_read_cb(inputDevice, touchpad_read);             //---+---does not work with this or without this functions
    lv_indev_enable(inputDevice, true);                           //---

    background = lv_image_create(lv_screen_active());
    lv_image_set_src(background, &Horo);
    lv_obj_align(background, LV_ALIGN_CENTER, 0, 0);
}

void initTouch()
{
  //Initialise TFT Touch
  uint16_t calData[5] = { 242, 3583, 325, 3617, 4 };
  tft->setTouch(calData);

  Serial.println("Touch Initialised");
  tft->println("Touch Initialised");
}

void initRTC()
{
  Serial.println("RTC starting ");
  tft->println("RTC starting ");
  for (int i = 0;i<20 && rtc.begin() != 0; ++i) 
  {
    Serial.print(".");
    tft->print(".");
    delay(500);
  }

  if(rtc.begin() == 0)
  {
    Serial.println("RTC started ");
    tft->println("RTC started ");
  }
  else
  {
    Serial.println("RTC failed ");
    tft->println("RTC failed ");
  }
  //rtc.setHourSystem(rtc.e24hours);//Set display format only needed for first time after new battery is added
  //rtc.setTime(2024,2,12,12,22,0);//Initialize time
}

void initI2C()
{
  Wire.setPins(I2C_SDA, I2C_SCL);

  delay(300);
    SensorBME680::init(tft);
  delay(300);

  initRTC();
  delay(100);
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
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Connecting to WiFi\n");
  tft->printf("Connecting to WiFi\n");

  // Wait for connection
  for (int i = 0;i<20 && WiFi.status() != WL_CONNECTED; ++i) 
  {
    delay(500);
    Serial.print(".");
    tft->print(".");
  }

  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.printf("\nConnected to %s\nIP address:\n%s\n", WIFI_SSID, WiFi.localIP().toString().c_str());
    tft->printf("\nConnected to %s\nIP address:\n%s\n", WIFI_SSID, WiFi.localIP().toString().c_str());
  }
  else
  {
    Serial.printf("WiFi disconected code:%d\n", WiFi.status());
    tft->printf("WiFi disconected code:%d\n", WiFi.status());
  }
  
}

void initFTP()
{
  if(WiFi.status() == WL_CONNECTED)
  {
    ftpSrv.setCallback(_callback);
    ftpSrv.setTransferCallback(_transferCallback);
    ftpSrv.begin(FTP_LOGIN, FTP_PASSWORD);    //username, password for ftp.   (default 21, 50009 for PASV)
  }
  else
  {
    Serial.printf("FTP not initialized\n");
    tft->printf("FTP not initialized\n");
  }
}

void setup() 
{
  // Use serial port
  Serial.begin(115200);

  initLVGL();
  delay(100);

  initTouch();
  delay(100);

  initI2C();
  delay(100);

  initSDMMC();
  delay(100);

  initWiFi();
  delay(100);

  initFTP();
  delay(100);

  LabelTemperature  = new DisplayLabel(&defaultLabelStyle, 5, 195);
  LabelHumidity     = new DisplayLabel(&defaultLabelStyle, 5, 235);
  LabelPressure     = new DisplayLabel(&defaultLabelStyle, 5, 275);
  LabelTime         = new DisplayLabel(&defaultLabelStyle, 59,  5);
  //LabelDate         = new DisplayLabel(&defaultLabelStyle, 73,  5);

  LabelIAQ          = new DisplayLabelIAQ(5, 5);

  Serial.println("Software start in:");
  tft->println("Software start in:");
  for (size_t i = 5U; i > 0; i--)
  {
    Serial.println(i);
    tft->println(i);
    delay(1000);
  }

  xMutexLabelUpdate = xSemaphoreCreateMutex();

  xTaskCreate(taskI2C,
              "I2C",
              STACK_SIZE_I2C,
              nullptr,
              10u,
              &xHandleI2C);

  xTaskCreate(taskFTP,
              "FTP",
              STACK_SIZE_FTP,
              nullptr,
              3u,
              &xHandleFTP);
}

uint64_t upTime = 0U;

void loop(void) 
{
  upTime = millis();

  if( xSemaphoreTake( xMutexLabelUpdate, portMAX_DELAY ) == pdTRUE )
  {
    lv_timer_handler();

    xSemaphoreGive( xMutexLabelUpdate);
  }

  delay(1);
  lv_tick_inc((millis() - upTime));
}

// Code to run a screen calibration, not needed when calibration values set in setup()
/*
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // Calibrate
  tft->fillScreen(TFT_BLACK);
  tft->setCursor(20, 0);
  tft->setTextFont(2);
  tft->setTextSize(1);
  tft->setTextColor(TFT_WHITE, TFT_BLACK);

  tft->println("Touch corners as indicated");

  tft->setTextFont(1);
  tft->println();

  tft->calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

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
  Serial.print("  tft->setTouch(calData);");
  Serial.println(); Serial.println();

  tft->fillScreen(TFT_BLACK);
  
  tft->setTextColor(TFT_GREEN, TFT_BLACK);
  tft->println("Calibration complete!");
  tft->println("Calibration code sent to Serial port.");

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