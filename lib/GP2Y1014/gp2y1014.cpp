#include "Arduino.h"
#include "gp2y1014.h"

GP2Y1014::GP2Y1014(int ledPin){
    led_pin = ledPin;
    pinMode(led_pin, OUTPUT);
    adc1_config_width(ADC_WIDTH_BIT_11);                        //Range 10 - 0-1023 11 - 0-2047
    adc1_config_channel_atten(measure_channel, ADC_ATTEN_DB_11); //ADC_ATTEN_DB_11 = 0-3,6V (or 3.9V??)
    esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_11, &characteristics);
}

float GP2Y1014::getDustDensity() {
	digitalWrite(led_pin, LOW);
	delayMicroseconds(SAMPLINGTIME);

    uint32_t mVoltRaw = adc1_to_voltage(ADC1_CHANNEL_0, &characteristics);
	delayMicroseconds(DELTATIME);

	digitalWrite(led_pin, HIGH);
	delayMicroseconds(SLEEPTIME);

	// calc dust density
    // 0.6V = 0ppm (output voltage at no dust)
    // +0.5V per +100ug/m3
    // 0.5V = 100ppm

    return mVoltRaw;

    // if(mVoltRaw <= 600){
    //     return 0.;
    // }


	// float dust = (0.17 * (mesured * (inputvolts / analog_bit_num)) - 0.1) * 1000.;
	// if( dust<0 )	dust=0.;
	
	// return dust;
}
