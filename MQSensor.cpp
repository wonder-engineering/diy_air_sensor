// Copyright 2020 Brett M. Leedy

#include "MQSensor.hh"

uint16_t MQSensor::read_sensor() {
  this->last_raw = analogRead(this->pin);
  return this->last_raw;
}
