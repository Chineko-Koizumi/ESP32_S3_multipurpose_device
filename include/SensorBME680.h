#ifndef SENSORBME680_H
#define SENSORBME680_H

#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library

#include "bsec.h"

class SensorBME680
{
private:

static Bsec iaqSensor;
static uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE];
static uint16_t stateUpdateCounter;

static const uint8_t bsec_config_iaq[IAQ_CONFIG_ITEMS_COUNT];

    SensorBME680()  = delete;
    ~SensorBME680() = delete;

    static void checkIaqSensorStatus(void);
    static void loadState(void);
    static void updateState(void);

public:
    static void init(TFT_eSPI* ScreenPtr);
    static bool run();

    static float getTemperature();
    static float getHumidity();
    static float getPressure();
    static float getStaticIAQ();
    static uint8_t getStaticIAQaccuracy();
    static float getIAQ();
    static uint8_t getIAQaccuracy();

};

#endif