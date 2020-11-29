// Copyright 2020 Brett M. Leedy

/*
  SensorArray class

  This class manages the list of sensors which are configured and
    provides a single location to call sense() to refresh the data.
    It stores sensed data in the SensorState class.

  It is aware of Sensor types, but not anything about particular
    sensor implementations.

  Modify this class if you want to change the order sensors are
    read or otherwise change the way sensors are handled.
*/

#ifndef SRC_SENSORARRAY_HH_
#define SRC_SENSORARRAY_HH_

#ifdef IN_TESTING
// just skip progmem macro for test builds
#define PROGMEM
#include "../test/Stub_Arduino.h"
#include "../test/Stub_Serial.h"
#include "../test/Stub_LiquidCrystal_I2C.hh"
#include "../test/File_mock.h"
#else
#include "Arduino.h"
#endif

#include "Sensor.hh"
#include "SensorState.hh"

/*  This SensorList class would be better replaced with a deque, but
    since AVR does not support STL (can't compile) this is implemented
    in lieu o unsupported STL containers.
*/
class SensorList {
 public:
  SensorList() {this->num_sensors = 0;}
  bool push_back(Sensor* new_sensor) {
    if (this->num_sensors >= SENSORSTATE_MAX_NUM_SENSORS)
      return false;
    this->list[this->num_sensors] = new_sensor;
    this->num_sensors++;
    return true;
  }
  uint8_t size() {return this->num_sensors;}
  uint8_t begin() {return 0;}
  uint8_t end() {return (this->num_sensors-1);}

  Sensor * operator[](uint8_t index) {return this->list[index];}

 private:
  uint8_t num_sensors;
  Sensor * list[SENSORSTATE_MAX_NUM_SENSORS];
};

class SensorArray {
 public:
  virtual bool add_sensor(Sensor * newsens);
  virtual void sense_all(SensorState * state);
  virtual void log_all_serial_only();
  void write_sensor_configs(SensorState * state);
  void read_sensor_configs(SensorState * state);
  SensorArray();

 private:
  SensorList sensors;
  void log_serial(uint8_t id);
  uint16_t sense(uint8_t id);
  uint16_t get_sensor_value(uint8_t sensor_id);

 protected:
  uint8_t get_num_sensors() {return sensors.size();}
  uint16_t get_sensor_avg(uint8_t sensor_id);
  uint16_t get_sensor_raw(uint8_t sensor_id);
  void get_short_name(uint8_t i, char buffer[], uint8_t buffer_size ) {
    sensors[i]->get_short_name(buffer, buffer_size);
  }
};
#endif  // SRC_SENSORARRAY_HH_
