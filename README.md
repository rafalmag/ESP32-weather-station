# ESP32_weather_station

## Hardware used:
- ESP32 with OLED display (connected internally to pins 4, 5)
- PMS5003 connected to 3 (RX), 1 (TX) (Serial(0) - same as used for flashing, so the sensor must be disconnected during programming)
- Sharp GP2Y10 connected to 39 (VN) (ADC - through voltage divider) and 14 (through logic converter)
