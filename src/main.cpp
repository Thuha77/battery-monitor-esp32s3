#include <Arduino.h>
#include "battery_monitor.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  batteryMonitorInit();
}

void loop() {
  float pct = getBatteryPercentage();

  Serial.printf("Battery: %.1f %%\n", pct);

  delay(1000);
}