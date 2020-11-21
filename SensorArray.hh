// Copyright 2020 Brett M. Leedy

#ifndef GAS_SENSOR_ANALOGSENSOR_H_
#define GAS_SENSOR_ANALOGSENSOR_H_

#include <stdio.h>
#include <string.h>


#ifdef IN_TESTING
// just skip progmem macro for test builds
#define PROGMEM
#include "test/Stub_Arduino.h"
#include "test/Stub_Serial.h"
#include "test/Stub_LiquidCrystal_I2C.hh"
#include "test/File_mock.h"

#else
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include "Arduino.h"
#endif


#include "Sensor.hh"

#define SHORT_NAME_LEN  5
#define MAX_NUM_SENSORS 6

#define MQ_DEFAULT_LAST_VALUE 0
#define MQ_DEFAULT_AVG_VALUE  0.0

//todo: pull lcd awareness out of the senso


class SensorList {  // in lieu o unsupported STL containers
 public:
  SensorList(){}
  bool push_back(Sensor* new_sensor) {
    if(this->num_sensors >= MAX_NUM_SENSORS)
      return false;
    this->list[this->num_sensors] = new_sensor;
    return true;
  }
  uint8_t size(){return this->num_sensors;}
  uint8_t begin(){return 0;}
  uint8_t end(){return (this->num_sensors-1);}

  Sensor * operator [](uint8_t index){return this->list[index];};
  
  private:
   uint8_t num_sensors;
   Sensor * list[MAX_NUM_SENSORS];
};


//todo: pull lcd awareness out of the sensor
class SensorArray {
  SensorList sensors;  // deque has better memory and
                            // access properties for this use case
 public:
  explicit SensorArray();
  bool add_sensor(Sensor * newsens);
  void sense_all();
  void log_all_serial_only();

  uint16_t get_sensor_avg(uint8_t sensor_id);
  uint16_t get_sensor_raw(uint8_t sensor_id);
  uint16_t get_sensor_value(uint8_t sensor_id);
  uint8_t get_num_sensors() {return sensors.size();}
  void get_short_name(uint8_t i, char buffer[], uint8_t buffer_size ) {sensors[i]->get_short_name(buffer, buffer_size);}

 private:
  void log_serial(uint8_t id);
  uint16_t sense(uint8_t id);
};
#endif  // GAS_SENSOR_ANALOGSENSOR_H_
