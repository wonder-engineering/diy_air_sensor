// Copyright 2020 Brett M. Leedy

#include "AnalogSensor.h"

AnalogSensor::AnalogSensor() {
}


bool AnalogSensor::add_sensor(Sensor * newsens) {
  if (this->sensors.size() >= MAX_NUM_SENSORS)
    return false;  // cannot add sensor. take no action
  sensors.push_back(newsens);

  // Init the analog input pin
  pinMode(newsens->get_pin(), INPUT);

  char buffer[SENSOR_SHORT_NAME_LEN];
  newsens->get_short_name(buffer, SENSOR_SHORT_NAME_LEN);
  Serial.print(buffer); Serial.println(F(" added!"));

  return true;
}


void AnalogSensor::sense_all() {
  for (std::deque <Sensor *> :: iterator sensor = sensors.begin();
        sensor != sensors.end();
        ++sensor) {
    (*sensor)->sense();
  }
}

void AnalogSensor::log_all_serial_only() {
  for (uint8_t i = 0; i < this->sensors.size(); i++) {
    this->log_serial(i);
  }
}

void AnalogSensor::log_serial(uint8_t sensor_id) {
  Serial.print(this->sensors[sensor_id]->get_pin());
  Serial.print(F("| "));
  char buffer[SENSOR_SHORT_NAME_LEN];
 sensors[sensor_id]->get_short_name(buffer, SENSOR_SHORT_NAME_LEN);
  Serial.print(buffer);
  Serial.print(F("- avg: "));
  Serial.print(this->sensors[sensor_id]->get_sensor_avg(), 1);
  Serial.print(F("- raw: "));
  Serial.print(this->sensors[sensor_id]->get_last_raw());
  Serial.print(F(" | last: "));
  Serial.println(this->sensors[sensor_id]->get_last_value());
}


uint16_t AnalogSensor::get_sensor_avg(uint8_t sensor_id) {
  return this->sensors[sensor_id]->get_sensor_avg();
}

uint16_t AnalogSensor::get_sensor_raw(uint8_t sensor_id) {
  return this->sensors[sensor_id]->get_last_raw();
}

uint16_t AnalogSensor::get_sensor_value(uint8_t sensor_id) {
  return this->sensors[sensor_id]->get_last_value();
}
