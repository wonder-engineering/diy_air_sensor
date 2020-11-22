// Copyright 2020 Brett M. Leedy

#include "SensorArray.hh"

SensorArray::SensorArray() {
}


bool SensorArray::add_sensor(Sensor * newsens) {
  if (this->sensors.size() >= SENSORSTATE_MAX_NUM_SENSORS)
    return false;  // cannot add sensor. take no action
  sensors.push_back(newsens);

  // Init the analog input pin
  pinMode(newsens->get_pin(), INPUT);

  char buffer[SENSOR_SHORT_NAME_LEN];
  newsens->get_short_name(buffer, SENSOR_SHORT_NAME_LEN);
  Serial.print(buffer); Serial.println(F(" added!"));

  return true;
}

void SensorArray::sense_all(SensorState * state) {
  for (uint8_t sensor_id = sensors.begin(); sensor_id < sensors.end(); sensor_id++) {
    sensors[sensor_id]->sense();

    // for each sensor, store it in the state DB
    state->sensor[sensor_id].data.value = sensors[sensor_id]->get_last_value();
    state->sensor[sensor_id].data.raw = sensors[sensor_id]->get_last_raw();
  }

  // store data to state
  state->device.num_sensors = sensors.size();
  
}

void SensorArray::log_all_serial_only() {
  for (uint8_t i = 0; i < this->sensors.size(); i++) {
    this->log_serial(i);
  }
}

void SensorArray::log_serial(uint8_t sensor_id) {
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


uint16_t SensorArray::get_sensor_avg(uint8_t sensor_id) {
  return this->sensors[sensor_id]->get_sensor_avg();
}

uint16_t SensorArray::get_sensor_raw(uint8_t sensor_id) {
  return this->sensors[sensor_id]->get_last_raw();
}

uint16_t SensorArray::get_sensor_value(uint8_t sensor_id) {
  return this->sensors[sensor_id]->get_last_value();
}

void SensorArray::write_sensor_configs(SensorState * state){
  // write all the config values of the sensors
  // used to initialize the sensor configs
  for (uint8_t sensor_id = sensors.begin(); sensor_id < sensors.end(); sensor_id++) {
    sensors[sensor_id]->get_short_name(state->sensor[sensor_id].config.shortname, SENSOR_SHORT_NAME_LEN);
  }
}

void SensorArray::read_sensor_configs(SensorState * state){
  // write all the config values that might have been manipulated by external,
  //   like an interactive menu.
}
