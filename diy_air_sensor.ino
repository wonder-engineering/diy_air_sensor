// Copyright 2020 Brett M. Leedy

#include <Arduino.h>
#include "DiyAirSensor.hh"


// Classes for main components
DiyAirSensor * diy_air_sensor;

void setup() {
  diy_air_sensor = new DiyAirSensor();
}

void loop() {
  diy_air_sensor->loop();
}
