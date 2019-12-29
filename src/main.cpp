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
// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
const int ledPwmPin = 25;//gpio25

// PMS5003
#include <PMS.h>
// violet wire 5V
// orange wire GND
PMS pms(Serial2);      // blue wire - TX (17), green wire - RX (16)
int pmsEnabledPin = 4; // white wire
#include "pmsCalc.h"
PmsCalc pmsCalc(pms);

// fastled
#include "FastLED.h" // FastLED library
#include "palettesFunctions.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define NEOPIXEL_DATA_PIN 2
#define LED_TYPE NEOPIXEL
#define NUM_LEDS 8

// Initialize LED array.
struct CRGB leds[NUM_LEDS];

// photoresistor ADC + LCD bright DAC
esp_adc_cal_characteristics_t characteristics;
#include <driver/dac.h>

void bmeInit()
{
    while (!bme.begin())
    {
        DebugPrintln("Could not find BME280 sensor!");
        delay(1000);
        if (millis() >= 10000)
        {
            DebugPrintln("Could not find BME280 sensor, giving up");
            return;
        }
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
    // digitalWrite(pmsEnabledPin, HIGH);
    pms.wakeUp();

    // FastLED
    LEDS.addLeds<LED_TYPE, NEOPIXEL_DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(11);
    set_max_power_in_volts_and_milliamps(5, 300); // FastLED Power management set at 5V, 300mA.
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    digitalWrite(LED, HIGH);

    // photoresistor ADC + LCD bright DAC
    // (same as in CO2)
    esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_11, &characteristics);
    // pin 39
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11); //ADC_ATTEN_DB_11 = 0-3,6V (or 3.9V??)
    // dac_output_enable(DAC_CHANNEL_1);
     ledcSetup(ledChannel, freq, resolution);
     ledcAttachPin(ledPwmPin, ledChannel);
}

void loop()
{
    DebugPrintln("next loop");
    digitalWrite(LED, LOW);

    int adcCo2 = co2FromAdc.getCO2();
    int co2Perc = adcCo2 / 10; // 1000ppm = 100%, so: div by 1000 mul by 100% = 10

    float temperature(NAN), humidity(NAN), pressure(NAN);
    bme.read(pressure, temperature, humidity, tempUnit, presUnit);

    pmsCalc.updatePmReads();

    lcd.setCursor(0, 0);
    lcd.printf("Temp %2.1f*C Hum %2.0f%% ", temperature, humidity);
    lcd.setCursor(0, 1);
    lcd.printf("PM2.5: %3.0f ug/m3", pmsCalc.getPm2());
    lcd.setCursor(0, 2);
    lcd.printf("PM10 : %3.0f ug/m3", pmsCalc.getPm10());
    lcd.setCursor(0, 3);
    lcd.printf("CO2 %4d ppm (%3d%%) ", adcCo2, co2Perc);

    // Hum
    leds[0] = humToColor(humidity);
    leds[1] = leds[0];

    leds[2] = CRGB::Black;

    // pm
    leds[3] = pmToColor(pmsCalc.getPm2(), pmsCalc.getPm10());
    leds[4] = leds[3];

    leds[5] = CRGB::Black;

    // co2
    leds[6] = co2ppmToColor(adcCo2);
    leds[7] = leds[6];

    // photoresistor and brightness
    // read 39(VN) photoresistor
    int lightSenseMV = adc1_to_voltage(ADC1_CHANNEL_3, &characteristics);
    // DebugPrintln(String("ADC = ") + lightSenseMV + "mv");

    // lightSenseMV > 0 - very bright
    int neopixelBrightness = 11;
    int dutyCycle = 200;
    // int dacValue = 255;
    // artificial lights
    if (lightSenseMV > 1000)
    {
        neopixelBrightness = 8;
        // dacValue = 253;
        dutyCycle = 100;
    }
    // dim lights
    if (lightSenseMV > 1500)
    {
        neopixelBrightness = 5;
        // dacValue = 250;
        dutyCycle = 50;
    }
    // night, no lights
    if (lightSenseMV > 3000)
    {
        neopixelBrightness = 2;
        // dacValue = 245;
        dutyCycle = 10;
    }
    // DebugPrintln(String("DAC val = ") + dacValue);
    // write A18/DAC1/25 to set the LCD brightness
    // dac_output_voltage(DAC_CHANNEL_1, dacValue);
    ledcWrite(ledChannel, dutyCycle);
    FastLED.setBrightness(neopixelBrightness);

    FastLED.show(); // Power managed display

    digitalWrite(LED, HIGH);
    delay(5000);
}
