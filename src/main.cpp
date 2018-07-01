

// temp + hum
// https://github.com/finitespace/BME280/
#include <BME280I2C.h>

// co2
// https://github.com/rafalmag/ESP32-MH-Z14A
#include <co2FromAdc.h>

#include <Wire.h>

// LCD
#include <LiquidCrystal_I2C.h>

// Connected to PIN 5 and 4. (For I2C is ok to reuse the same pins by many devices, eg. oled)
BME280I2C bme; // Default : forced mode, standby time = 1000 ms
// Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off

BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_hPa);

#define LED 5 // on board LED for Lolin32
Co2FromAdc co2FromAdc;

//set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    Serial.begin(115200);

    co2FromAdc.init();

    lcd.begin(21, 22); // initialize the lcd with SDA 21 and SCL 22 pins
    lcd.backlight();

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
    digitalWrite(LED, HIGH);
}

void loop()
{
    Serial.println("next loop");
    digitalWrite(LED, LOW);
    int adcCo2 = co2FromAdc.getCO2();
    int co2Perc = adcCo2 / 10; // / 1000 * 100% = 10

    float temperature(NAN), humidity(NAN), pressure(NAN);

    bme.read(pressure, temperature, humidity, tempUnit, presUnit);

    lcd.setCursor(0, 0);
    // lcd.print(String("Temp ") + temperature + "*C Hum " + humidity + "%");
    lcd.printf("Temp %2.1f*C Hum %2.0f%% ", temperature, humidity);
    lcd.setCursor(0, 1);
    lcd.print("PM2.5: not impl");
    lcd.setCursor(0, 2);
    lcd.print("PM10 : not impl");
    lcd.setCursor(0, 3);
    lcd.printf("CO2 %4d ppm (%3d%%) ",adcCo2,co2Perc);

    digitalWrite(LED, HIGH);
    delay(5000);
}