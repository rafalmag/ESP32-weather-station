#ifndef GP2Y1014_h_
#define GP2Y1014_h_

#include <driver/adc.h>
#include "esp_adc_cal.h"
#define V_REF 1100

/**
   ADC1_CHANNEL_3 pin hardcoded, ie. GPIO_39 (aka VN)
 */
class GP2Y1014
{
  public:
    GP2Y1014(int ledPin);

    float getDustDensity();

  protected:
  private:
    int led_pin;
    adc1_channel_t measure_channel = ADC1_CHANNEL_3;

    esp_adc_cal_characteristics_t characteristics;

    static const int SAMPLINGTIME = 280;
    static const int DELTATIME = 40; // = 0.32ms - 0.28ms
    static const int SLEEPTIME = 9680; // = 10ms - 0.32ms
};

#endif