// #include <Arduino.h>
#include "SSD1306Wire.h"
#include "DHTesp.h"
#include "font.h"

DHTesp dht;

// Initialize the OLED display using Wire library
// according to http://www.instructables.com/id/ESP32-With-Integrated-OLED-WEMOSLolin-Getting-Star/

// This ESP’s dedicated I2C pins are on GPIO 5 and 4 for data and clock respectively.
SSD1306Wire  display(0x3c, 5, 4);

void setup() {
      Serial.begin(115200);
  Serial.println("start");
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");

    display.init();
      dht.setup(16); // Connect DHT sensor to GPIO 14 (18?)

  display.flipScreenVertically();
}

void draw() {
    // 128 x 64
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(Monospaced_plain_10);

    float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);

    display.drawString(0, 1*16, String("temp    : ") + temperature + "*C");
    display.drawString(0, 0*16, String("humidity: ") + humidity + "%");
    display.drawString(0, 2*16, String("heat Idx: ") + heatIndex + "*C");
    display.drawString(0, 3*16, "Hello world 3");
}

void loop() {
    Serial.println("loop");

    display.clear();
    draw();
    display.display();
    delay(dht.getMinimumSamplingPeriod());
}