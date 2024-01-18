#include <EEPROM.h>

#include "InlineDefinitions.h"
#include "SensorBME680.h"

Bsec SensorBME680::iaqSensor;
uint8_t SensorBME680::bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
uint16_t SensorBME680::stateUpdateCounter = 0;

const uint8_t SensorBME680::bsec_config_iaq[IAQ_CONFIG_ITEMS_COUNT] = { 
#include "config/generic_33v_3s_28d/bsec_iaq.txt" 
};

void SensorBME680::init(TFT_eSPI* ScreenPtr)
{
    Wire.setPins(I2C_SDA, I2C_SCL);
    iaqSensor.begin(BME68X_I2C_ADDR_HIGH, Wire);
    String LibraryVersion = "BSEC library version" + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
    Serial.println(LibraryVersion);
    ScreenPtr->println(LibraryVersion);
    checkIaqSensorStatus();

    iaqSensor.setTemperatureOffset(TEMPERATURE_OFFSET);
    iaqSensor.setConfig(bsec_config_iaq);
    checkIaqSensorStatus();

    loadState();
    Serial.println("EEPROM config loaded");
    ScreenPtr->println("EEPROM config loaded");

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
    ScreenPtr->println("BME680 Initialised");
}

bool SensorBME680::run()
{   
    bool SensorData = iaqSensor.run();

    if(SensorData)
    {
        updateState();
    }
    else
    {
        checkIaqSensorStatus();
    }

    return SensorData;
}

float SensorBME680::getTemperature()
{
   return iaqSensor.temperature; 
}

float SensorBME680::getHumidity()
{
    return iaqSensor.humidity; 
}

float SensorBME680::getPressure()
{
    return iaqSensor.pressure; 
}

float SensorBME680::getStaticIAQ()
{
    return iaqSensor.staticIaq;
}

uint8_t SensorBME680::getStaticIAQaccuracy()
{
   return iaqSensor.staticIaqAccuracy; 
}

float SensorBME680::getIAQ()
{
    return iaqSensor.iaq;
}

uint8_t SensorBME680::getIAQaccuracy()
{
   return iaqSensor.iaqAccuracy; 
}

void SensorBME680::checkIaqSensorStatus()
{
    if (iaqSensor.bsecStatus != BSEC_OK) 
    {
        if (iaqSensor.bsecStatus < BSEC_OK) 
        {
            Serial.print("BSEC error code : ");
            Serial.println(iaqSensor.bsecStatus);
            while(true); /* Halt in case of failure */
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
            while(true); /* Halt in case of failure */
        } 
        else 
        {
            Serial.print("BME68X warning code : ");
            Serial.println(iaqSensor.bme68xStatus);
        }
    }
}

void SensorBME680::loadState()
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

void SensorBME680::updateState()
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
            Serial.print(bsecState[i], HEX);
        }
        Serial.println();

        EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
        EEPROM.commit();
    }
}