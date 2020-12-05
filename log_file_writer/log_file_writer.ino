// Copyright 2020 Brett M. Leedy
/*
 * diy_air_sensor
 *
 * This contains the Arduino main() callbacks.
 * See DiyAirSensor class for implementation.
*/

#include <Arduino.h>
#include "LogFileWriter.hh"


// Classes for main components
LogFileWriter * log_file_writer;

void setup() {
  Serial.begin(115200);
  Serial.println(F("BEGIN----------------------"));
  log_file_writer = new LogFileWriter();
}

void loop() {
  log_file_writer->listen_for_line();
}
