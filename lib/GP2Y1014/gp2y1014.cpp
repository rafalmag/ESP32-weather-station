#include "Arduino.h"
#include "gp2y1014.h"

GP2Y1014::GP2Y1014(int ledPin)
{
    led_pin = ledPin;
    pinMode(led_pin, OUTPUT);
    adc1_config_width(adc_bits_width);                         
    adc1_config_channel_atten(adc1_channel, adc_atten);
    // esp_adc_cal_get_characteristics(adc_default_vref, adc_atten, adc_bits_width, &characteristics);

    esp_adc_cal_characterize(adc_unit, adc_atten, adc_bits_width, adc_default_vref, &characteristics);
}

float GP2Y1014::readDustDensity()
{
    uint32_t mVolt = 0;
    for (int i = 0; i < SAMPLES; i++)
    {
        digitalWrite(led_pin, HIGH); // on
        delayMicroseconds(SAMPLING_TIME_US);

        uint32_t mVoltRaw;
        esp_adc_cal_get_voltage(adc_channel, &characteristics, &mVoltRaw);
        delayMicroseconds(DELTA_TIME_US);

        digitalWrite(led_pin, LOW); // off
        delayMicroseconds(SLEEP_TIME_US);
        delay(EXTRA_SLEEP_TIME_MS);
        mVolt += mVoltRaw;
    }
    mVolt = mVolt / SAMPLES;

    // calc dust density
    // 0.6V = 0ppm (output voltage at no dust)
    // +0.5V per +100ug/m3
    // 0.5V = 100ppm

    float adjustedVoltageV = mVolt * 5.0 / 3.0 * 0.001;
    
    /**
     * https://www.pocketmagic.net/sharp-gp2y1010-dust-sensor/
     * y=ax+b
     * A(0;-100), B(3.5;500)
     * b=-100
     * 500=a*3.5-100
     * a=600/3.5 = 171.4
     * y=171.4*x-100
     * 
     * 0=171.4*x-100
     * x=100/171.4 = 0.583
     */
    if (adjustedVoltageV <= 0.583)
    {
        return 0.;
    }
    else
    {
        return 171.4 * adjustedVoltageV - 100;
    }
    
}
