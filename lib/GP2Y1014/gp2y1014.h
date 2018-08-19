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

  // returns ug/m^3
  float readDustDensity();

protected:
private:
  int led_pin;
  adc1_channel_t adc1_channel = ADC1_CHANNEL_3; // GPIO_39 (aka VN)
  adc_channel_t adc_channel = ADC_CHANNEL_3;
  adc_atten_t adc_atten = ADC_ATTEN_DB_0; // ADC_ATTEN_DB_0 = 0-1.1V
  adc_bits_width_t adc_bits_width = ADC_WIDTH_BIT_12; // Range 10 - 0-1023, 11 - 0-2047, 12 - 0-4095
  adc_unit_t adc_unit = ADC_UNIT_1;
  uint32_t adc_default_vref = V_REF;

  esp_adc_cal_characteristics_t characteristics;

  // according to datasheet
  // sampling time should be ~10ms
  static const int SAMPLING_TIME_US = 280;
  static const int DELTA_TIME_US = 40;   // 40 = 0.32ms - 0.28ms
  static const int SLEEP_TIME_US = 9680; // = 10ms - 0.32ms

  // https://cassiopeia.hk/dustsensor/
  static const int EXTRA_SLEEP_TIME_MS = 50; // 0 for GP2Y1010, 100 for GP2Y1014
  // after extra time it is 60ms
  static const int SAMPLES = 20;               // 20 for GP2Y1010, ?? for GP2Y1014
};

#endif