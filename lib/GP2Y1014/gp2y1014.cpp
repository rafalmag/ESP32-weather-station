#include "Arduino.h"
#include "gp2y1014.h"

GP2Y1014::GP2Y1014(int ledPin)
{
    led_pin = ledPin;
    pinMode(led_pin, OUTPUT);
    adc1_config_width(ADC_WIDTH_BIT_11);                         //Range 10 - 0-1023 11 - 0-2047
    adc1_config_channel_atten(measure_channel, ADC_ATTEN_DB_11); //ADC_ATTEN_DB_11 = 0-3,6V (or 3.9V??)
    // esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_11, &characteristics);

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_11, V_REF, &characteristics);
}

float GP2Y1014::readDustDensity()
{
    uint32_t mVolt = 0;
    for (int i = 0; i < SAMPLES; i++)
    {
        digitalWrite(led_pin, HIGH); // on
        delayMicroseconds(SAMPLING_TIME_US);

        uint32_t mVoltRaw;
        esp_adc_cal_get_voltage(ADC_CHANNEL_3, &characteristics, &mVoltRaw);
        delayMicroseconds(DELTA_TIME_US);

        digitalWrite(led_pin, LOW); // off
        delayMicroseconds(SLEEP_TIME_US);
        delay(EXTRA_SLEEP_TIME_MS);
        mVolt += mVoltRaw;
    }
    mVolt = mVolt / SAMPLES;

    //return mVolt * 5.0 / 3.3 ; //TODO breking change

    // calc dust density
    // 0.6V = 0ppm (output voltage at no dust)
    // +0.5V per +100ug/m3
    // 0.5V = 100ppm

    float adjustedVoltageV = mVolt /* * 5.0 / 3.3 */ * 0.001;
    
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
