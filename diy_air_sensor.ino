// Copyright 2020 Brett M. Leedy
/*
 * diy_air_sensor
 *
 * This contains the Arduino main() callbacks.
 * See DiyAirSensor class for implementation.
*/

#include <Arduino.h>
#include "src/DiyAirSensor.hh"


// Classes for main components
DiyAirSensor * diy_air_sensor;

void setup() {
  Serial.begin(115200);
  Serial.println(F("BEGIN----------------------"));
  diy_air_sensor = new DiyAirSensor();
}

void loop() {
  diy_air_sensor->loop();
}
