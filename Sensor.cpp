// Copyright 2020 Brett M. Leedy

#include "Sensor.hh"

// Generic sensor constructor
Sensor::Sensor(const char * short_name,
                    float accum_rate,
                    uint8_t pin,
                    uint16_t zero_adjust,
                    float gain) {
  strncpy(this->short_name, short_name, SENSOR_SHORT_NAME_LEN);
  this->accum_rate = accum_rate;
  this->pin = pin;
  this->zero_adjust = zero_adjust;
  this->gain = gain;
  this->last_raw = SENSOR_DEFAULT_LAST_VALUE;
  this->avg_value = SENSOR_DEFAULT_AVG_VALUE;
}

float Sensor::get_sensor_avg() {
  return this->avg_value * gain + zero_adjust;
}

float Sensor::get_last_value() {
  return last_raw * gain + zero_adjust;
}

uint16_t Sensor::get_last_raw() {
  return this->last_raw;
}

void Sensor::get_short_name(char * buffer, uint8_t buffer_size) {
  strncpy(buffer, this->short_name, buffer_size);
}

uint8_t Sensor::get_pin() {
  return this->pin;
}

uint16_t Sensor::sense() {
  this->last_raw = this->read_sensor();
  this->avg_value = this->avg_value*(1.0-this->accum_rate)
    + this->last_raw*this->accum_rate;
  return this->last_raw;
}
