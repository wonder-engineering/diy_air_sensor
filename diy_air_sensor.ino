// Copyright 2020 Brett M. Leedy
/*
 * diy_air_sensor
 *
 * This contains the Arduino main() callbacks.
 * See DiyAirSensor class for implementation.
*/

#include <Arduino.h>
#include "DiyAirSensor.hh"


// Classes for main components
DiyAirSensor * diy_air_sensor;

void setup() {
  diy_air_sensor = new DiyAirSensor(&Serial);
}

void loop() {
  diy_air_sensor->loop(&Serial, millis);
}
