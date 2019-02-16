// Toggle below line to enable / disabledebug to serial
#define SERIAL_DEBUG_ENABLED
#include "debug.h"

// temp + hum
// https://github.com/finitespace/BME280/
#include <BME280I2C.h>

// co2
// https://github.com/rafalmag/ESP32-MH-Z14A
#include <co2FromAdc.h>

#include <Wire.h>

// LCD
#include <LiquidCrystal_I2C.h>

// Connected to 21(SDA), 22(SCL). (For I2C is ok to reuse the same pins by many devices, eg. oled,lcd)
BME280I2C bme; // Default : forced mode, standby time = 1000 ms
// Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off

BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_hPa);

#define LED 5 // on board LED for Lolin32
Co2FromAdc co2FromAdc;

//set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

// PMS5003
#include <PMS.h>
// violet wire 5V
// orange wire GND
PMS pms(Serial2);      // blue wire - TX (17), green wire - RX (16)
int pmsEnabledPin = 4; // white wire
#include "pmsCalc.h"

void bmeInit()
{
    while (!bme.begin())
    {
        DebugPrintln("Could not find BME280 sensor!");
        delay(1000);
    }

#ifdef SERIAL_DEBUG_ENABLED
    switch (bme.chipModel())
    {
    case BME280::ChipModel_BME280:
        DebugPrintln("Found BME280 sensor! Success.");
        break;
    case BME280::ChipModel_BMP280:
        DebugPrintln("Found BMP280 sensor! No Humidity available.");
        break;
    default:
        DebugPrintln("Found UNKNOWN sensor! Error!");
    }
#endif
}

// GP2Y1014:
// "pin 1" white - 5V (through 150 Ohm to 5V; 220uF to GND)
// "pin 2" blue - GND
GP2Y1014 gp2y1014(14); // green "pin 3" - GPIO 14 (through logic conv)
// "pin 4" yellow - GND
// "pin 5" black - GPIO 39 (aka VN) (through logic conv) - GP2Y1014 lib will read it
// "pin 6" red - VCC - 5V

void setup()
{
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    Serial.begin(115200);

    co2FromAdc.init();

    // SDA 21, SCL 22 for I2C for lcd and bme
    Wire.begin();
    // LiquidCrystal_I2C
    lcd.init();
    lcd.backlight();

    bmeInit();

    // PMS5003
    Serial2.begin(9600); // 16,17
    digitalWrite(pmsEnabledPin, HIGH);
    pms.wakeUp();

    digitalWrite(LED, HIGH);
}

void loop()
{
    DebugPrintln("next loop");
    digitalWrite(LED, LOW);

    int adcCo2 = co2FromAdc.getCO2();
    int co2Perc = adcCo2 / 10; // 1000ppm = 100%, so: div by 1000 mul by 100% = 10

    float temperature(NAN), humidity(NAN), pressure(NAN);
    bme.read(pressure, temperature, humidity, tempUnit, presUnit);

    updatePmReads();

    lcd.setCursor(0, 0);
    lcd.printf("Temp %2.1f*C Hum %2.0f%% ", temperature, humidity);
    lcd.setCursor(0, 1);
    lcd.printf("PM2.5: %3.0f ug/m3", pm.pm2);
    lcd.setCursor(0, 2);
    lcd.printf("PM10 : %3.0f ug/m3", pm.pm10);
    lcd.setCursor(0, 3);
    lcd.printf("CO2 %4d ppm (%3d%%) ", adcCo2, co2Perc);

    digitalWrite(LED, HIGH);
    delay(5000);
}
