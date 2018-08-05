
// https://github.com/ThingPulse/esp8266-oled-ssd1306
#include "SSD1306Wire.h"

#include <PMS.h>
#include <gp2y1014.h>

// http://oleddisplay.squix.ch/
#include "font.h"

// WARN - PMS is using Serial RX/TX (1/3 pins) - so it must be disconnected during programming

// violet wire 5V
// orange wire GND
PMS pms(Serial); // blue wire - TX (1), green wire - RX (3)
int setPin = 16; // white wire

PMS::DATA data;

struct pms5003
{
    float pm1, pm2, pm10;
};

pms5003 pm;
float dust;

int readIter = 3;
bool updatePmReads()
{
    int reads = 0;
    uint16_t min1 = 0, max1 = 0, sum1 = 0,
             min2 = 0, max2 = 0, sum2 = 0,
             min10 = 0, max10 = 0, sum10 = 0;
    for (int i = 0; i < readIter; i++)
    {
        pms.requestRead();
        if (pms.read(data, 10000))
        {
            if (reads == 0)
            {
                min1 = data.PM_AE_UG_1_0;
                max1 = data.PM_AE_UG_1_0;
                sum1 = data.PM_AE_UG_1_0;
                min2 = data.PM_AE_UG_2_5;
                max2 = data.PM_AE_UG_2_5;
                sum2 = data.PM_AE_UG_2_5;
                min10 = data.PM_AE_UG_10_0;
                max10 = data.PM_AE_UG_10_0;
                sum10 = data.PM_AE_UG_10_0;
            }
            else
            {
                if (data.PM_AE_UG_1_0 < min1)
                    min1 = data.PM_AE_UG_1_0;
                if (max1 < data.PM_AE_UG_1_0)
                    max1 = data.PM_AE_UG_1_0;
                sum1 += data.PM_AE_UG_1_0;
                if (data.PM_AE_UG_2_5 < min2)
                    min2 = data.PM_AE_UG_2_5;
                if (max2 < data.PM_AE_UG_2_5)
                    max2 = data.PM_AE_UG_2_5;
                sum2 += data.PM_AE_UG_2_5;
                if (data.PM_AE_UG_10_0 < min10)
                    min10 = data.PM_AE_UG_1_0;
                if (max10 < data.PM_AE_UG_10_0)
                    max10 = data.PM_AE_UG_10_0;
                sum10 += data.PM_AE_UG_10_0;
            }
            reads++;
        }
    }
    if (reads > 2)
    {
        pm.pm1 = (float)(sum1 - min1 - max1) / (float)(reads - 2);
        pm.pm2 = (float)(sum2 - min2 - max2) / (float)(reads - 2);
        pm.pm10 = (float)(sum10 - min10 - max10) / (float)(reads - 2);
    }
    else
    {
        pm.pm1 = min1;
        pm.pm2 = min2;
        pm.pm10 = min10;
    }
    return reads > 0;
}

// Initialize the OLED display using Wire library
// according to http://www.instructables.com/id/ESP32-With-Integrated-OLED-WEMOSLolin-Getting-Star/

// This ESP’s dedicated I2C pins are on GPIO 5 and 4 for data and clock respectively.
SSD1306Wire display(0x3c, 5, 4);

// GP2Y1014:
// "pin 1" white - 5V (through 150 Ohm to 5V; 220uF to GND)
// "pin 2" blue - GND
GP2Y1014 gp2y1014(14); // green "pin 3" - GPIO 14 (through logic conv)
// "pin 4" yellow - GND
// "pin 5" black - GPIO 39 (aka VN) (through logic conv) - GP2Y1014 lib will read it
// "pin 6" red - VCC - 5V

void setup()
{
    Serial.begin(9600);
    display.init();
    display.flipScreenVertically();

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Monospaced_plain_10);

    display.clear();
    display.drawString(0, 0 * 16, "Hello! Sleep 5s");
    display.display();

    digitalWrite(setPin, HIGH);
    pms.wakeUp();
    delay(5000);

    display.clear();
    display.drawString(0, 0 * 16, "Hello! Ready!");
    display.display();
}

void draw()
{
    // 128 x 64
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Monospaced_plain_10);

    display.drawString(0, 0 * 16, String("PM 1.0: ") + pm.pm1 + "ug/m3");
    display.drawString(0, 1 * 16, String("PM 2.5: ") + pm.pm2 + "ug/m3");
    display.drawString(0, 2 * 16, String("PM 10 : ") + pm.pm10 + "ug/m3");
    display.drawString(0, 3 * 16, String("GP2Y1014: ") + dust + "ug/m3");
}

void loop()
{
    updatePmReads();
    dust = gp2y1014.readDustDensity();
    display.clear();
    draw();
    display.display();

    // pms.sleep();
    // digitalWrite(setPin, LOW);
    delay(1000);
}