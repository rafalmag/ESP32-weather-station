#include "Arduino.h"
#include "FastLED.h"
#include "Math.h"

// https://github.com/FastLED/FastLED/wiki/Gradient-color-palettes
// http://angrytools.com/gradient/

DEFINE_GRADIENT_PALETTE(co2_gp){
    0, 0, 0, 0,        // 0ppm, black
    47, 0, 0, 0,       // 470ppm, black
    48, 121, 188, 106, // 480ppm, green
    59, 121, 188, 106, // 590ppm, green
    60, 187, 207, 76,  // 600ppm, lime green
    90, 187, 207, 76,  // 900ppm, lime green
    100, 255, 153, 0,  // 1000ppm, orange
    130, 255, 153, 0,  // 1300ppm, orange
    150, 255, 0, 0,    // 1500ppm, red
    255, 255, 0, 0};   // +++, red
CRGBPalette16 co2_p = co2_gp;

CRGB co2ppmToColor(int ppm)
{
    TBlendType blendType = LINEARBLEND;
    uint8_t index;
    if (ppm >= 2500)
        index = 250;
    else if (ppm < 0)
        index = 0;
    else
        index = ppm / 10;

    return ColorFromPalette(co2_p, index, 255, blendType);
}

// green  121,188,106
// lime green 187,207,76
// yellow 238,194,11
// orange 242,147,5
// red-pink 232,65,111

// DEFINE_GRADIENT_PALETTE(pm2_gp){
//     0, 121, 188, 106,  // green
//     15, 121, 188, 106, // green
//     16, 187,207,76,   // lime green
//     30, 187,207,76,   // lime green
//     31, 238,194,11,   // yellow
//     55, 238,194,11,   // yellow
//     56, 242,147,5,     // orange
//     110, 242,147,5,    // orange
//     111, 232,65,111,    // red-pink
//     255, 232,65,111};   // red-pink
// CRGBPalette16 pm2_p = pm2_gp;

// DEFINE_GRADIENT_PALETTE(pm10_gp){
//     0, 121, 188, 106,  // green
//     25, 121, 188, 106, // green
//     26, 187,207,76,   // lime green
//     50, 187,207,76,   // lime green
//     51, 238,194,11,   // yellow
//     90, 238,194,11,   // yellow
//     91, 242,147,5,     // orange
//     180, 242,147,5,    // orange
//     181, 232,65,111,    // red-pink
//     255, 232,65,111};   // red-pink
// CRGBPalette16 pm2_p = pm2_gp;

DEFINE_GRADIENT_PALETTE(caqi_gp){
    0, 121, 188, 106,   // green
    25, 121, 188, 106,  // green
    26, 187, 207, 76,   // lime green
    50, 187, 207, 76,   // lime green
    51, 238, 194, 11,   // yellow
    75, 238, 194, 11,   // yellow
    76, 242, 147, 5,    // orange
    100, 242, 147, 5,   // orange
    101, 232, 65, 111,  // red-pink
    255, 232, 65, 111}; // red-pink
CRGBPalette16 caqi_p = caqi_gp;

// https://en.wikipedia.org/wiki/Air_quality_index
int linear(int aqiHigh, int aqiLow, float concHigh, float concLow, float concentration)
{
    // https://en.cppreference.com/w/c/numeric/math/rint
    return lrintf(((concentration - concLow) / (concHigh - concLow)) * (aqiHigh - aqiLow) + aqiLow);
}

/*
AQI
0 to 50  	Good	    Green   0,228,0
51 to 100	Moderate	Yellow  255,255,0
101 to 150	Unhealthy 	Orange  255,126,0
151 to 200	Unhealthy	Red     255,0,0
201 to 300	Very Unhlth	Purple  153,0,76
301 to 500	Hazardous	Maroon  126,0,35
*/
CRGB maroon = CRGB(126, 0, 35);

DEFINE_GRADIENT_PALETTE(aqi_gp){
    00, 0, 228, 0,
    50, 0, 228, 0,
    51, 255, 255, 0,
    100, 255, 255, 0,
    101, 255, 126, 0,
    150, 255, 126, 0,
    151, 255, 0, 0,
    200, 255, 0, 0,
    201, 153, 0, 76,
    255, 153, 0, 76};
CRGBPalette16 aqi_p = aqi_gp;

int pm2ToCaqi(float c)
{
    if (c >= 0 && c < 15)
        return linear(25, 0, 15, 0, c);
    else if (c >= 15 && c < 30)
        return linear(50, 25, 30, 15, c);
    else if (c >= 30 && c < 55)
        return linear(75, 50, 55, 30, c);
    else if (c >= 55 && c < 110)
        return linear(100, 75, 110, 55, c);
    else if (c >= 110)
        return 101;
    else
        return -1;
}

int pm10ToCaqi(float c)
{
    if (c >= 0 && c < 25)
        return linear(25, 0, 25, 0, c);
    else if (c >= 25 && c < 50)
        return linear(50, 25, 50, 25, c);
    else if (c >= 50 && c < 95)
        return linear(75, 50, 90, 50, c);
    else if (c >= 90 && c < 180)
        return linear(100, 75, 180, 90, c);
    else if (c >= 180)
        return 101;
    else
        return -1;
}

int pm2ToAqi(float c)
{
    if (c >= 0 && c < 12.1)
        return linear(50, 0, 12, 0, c);
    else if (c >= 12.1 && c < 35.5)
        return linear(100, 51, 35.4, 12.1, c);
    else if (c >= 35.5 && c < 55.5)
        return linear(150, 101, 55.4, 35.5, c);
    else if (c >= 55.5 && c < 150.5)
        return linear(200, 151, 150.4, 55.5, c);
    else if (c >= 150.5 && c < 250.5)
        return linear(300, 201, 250.4, 150.5, c);
    else if (c >= 250.5 && c < 350.5)
        return linear(400, 301, 350.4, 250.5, c);
    else if (c >= 350.5 && c < 500.5)
        return linear(500, 401, 500.4, 350.5, c);
    else if (c >= 500.5)
        return 500;
    else
        return -1;
}

int pm10ToAqi(float c)
{
    if (c >= 0 && c < 55)
        return linear(50, 0, 54, 0, c);
    else if (c >= 55 && c < 155)
        return linear(100, 51, 154, 55, c);
    else if (c >= 155 && c < 255)
        return linear(150, 101, 254, 155, c);
    else if (c >= 255 && c < 355)
        return linear(200, 151, 354, 255, c);
    else if (c >= 355 && c < 425)
        return linear(300, 201, 424, 355, c);
    else if (c >= 425 && c < 505)
        return linear(400, 301, 504, 425, c);
    else if (c >= 505 && c < 605)
        return linear(500, 401, 604, 505, c);
    else if (c >= 605)
        return 500;
    else
        return -1;
}

CRGB pmToColor(float pm2, float pm10)
{
    int aqi = max(pm2ToAqi(pm2), pm10ToAqi(pm10));
    if (aqi < 0)
        return CRGB::Black;
    else if (aqi <= 255)
    {
        TBlendType blendType = LINEARBLEND;
        return ColorFromPalette(aqi_p, aqi, 255, blendType);
    }
    else
        return maroon;
}

CRGB pmToColor2(float pm2, float pm10)
{
    int caqi = max(pm2ToCaqi(pm2), pm10ToCaqi(pm10));
    if (caqi < 0)
        return CRGB::Black;
    else
    {
        TBlendType blendType = LINEARBLEND;
        return ColorFromPalette(caqi_p, caqi, 255, blendType);
    }
}

// dry - yellow
// 30-55 ok - green
// wet - blue 109,213,237

DEFINE_GRADIENT_PALETTE(hum_gp){
    0, 238, 194, 11,     // yellow
    29, 238, 194, 11,    // yellow
    30, 121, 188, 106,   // green
    55, 121, 188, 106,   // green
    56, 109, 213, 237,   // blue
    255, 109, 213, 237}; // blue
CRGBPalette16 hum_p = hum_gp;

CRGB humToColor(int hum)
{
    if (hum >= 0 && hum <= 100)
    {
        TBlendType blendType = LINEARBLEND;
        return ColorFromPalette(hum_p, hum, 255, blendType);
    }
    else
    {
        return CRGB::Black;
    }
}
