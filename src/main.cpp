
// https://github.com/ThingPulse/esp8266-oled-ssd1306
#include "SSD1306Wire.h"
// https://github.com/finitespace/BME280/
#include <BME280I2C.h>
#include <EnvironmentCalculations.h>
#include <Wire.h>
// http://oleddisplay.squix.ch/
#include "font.h"

// Connected to PIN 5 and 4. (For I2C is ok to reuse the same pins by many devices, eg. oled)
BME280I2C bme; // Default : forced mode, standby time = 1000 ms
               // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off

// altitude for Wroclaw
#define altitudeM 117.17

// Initialize the OLED display using Wire library
// according to http://www.instructables.com/id/ESP32-With-Integrated-OLED-WEMOSLolin-Getting-Star/

// This ESP’s dedicated I2C pins are on GPIO 5 and 4 for data and clock respectively.
SSD1306Wire display(0x3c, 5, 4);

void setup()
{
    Serial.begin(115200);
    Serial.println("start");
    Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");

    display.init();
    display.flipScreenVertically();

    Wire.begin();
    while (!bme.begin())
    {
        Serial.println("Could not find BME280 sensor!");
        delay(1000);
    }

    switch (bme.chipModel())
    {
    case BME280::ChipModel_BME280:
        Serial.println("Found BME280 sensor! Success.");
        break;
    case BME280::ChipModel_BMP280:
        Serial.println("Found BMP280 sensor! No Humidity available.");
        break;
    default:
        Serial.println("Found UNKNOWN sensor! Error!");
    }
}

void draw()
{
    // 128 x 64
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Monospaced_plain_10);

    float temperature(NAN), humidity(NAN), pressure(NAN);

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_hPa);

    EnvironmentCalculations::AltitudeUnit envAltUnit = EnvironmentCalculations::AltitudeUnit_Meters;
    EnvironmentCalculations::TempUnit envTempUnit = EnvironmentCalculations::TempUnit_Celsius;

    bme.read(pressure, temperature, humidity, tempUnit, presUnit);

    float seaLevel = EnvironmentCalculations::EquivalentSeaLevelPressure(altitudeM, temperature, pressure, envAltUnit, envTempUnit);

    display.drawString(0, 1 * 16, String("temp    : ") + temperature + "*C");
    display.drawString(0, 0 * 16, String("humidity: ") + humidity + "%");
    display.drawString(0, 2 * 16, String("pressure: ") + pressure + "hPa");
    display.drawString(0, 3 * 16, String("see pres: ") + seaLevel + "hPa");
}

void loop()
{
    Serial.println("next loop");

    display.clear();
    draw();
    display.display();
    delay(1000);
}