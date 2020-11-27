// Copyright 2020 Brett M. Leedy
/*
  Sensor class

  This is the interface for all single-output sensor types.

  To implement, create a derived class which implements the read_sensor()
    method.  The read_sensor() method must do two things:
     * store the most recent raw sensor reading in last_raw
     * return the most recent raw sensor reading
*/

#ifndef SENSOR_HH_
#define SENSOR_HH_
// todo: use c++ .hh header conventions

#include <stdio.h>
#include <string.h>

#ifdef IN_TESTING
#include "test/Stub_Arduino.h"
#else
#include "Arduino.h"
#endif

#define SENSOR_SHORT_NAME_LEN  5

#define SENSOR_DEFAULT_LAST_VALUE 0
#define SENSOR_DEFAULT_AVG_VALUE  0.0

// Sensor interface for a type of analog sensor
class Sensor {
 public:
  // sensor external  interface
  uint16_t sense();
  float get_sensor_avg();
  float get_last_value();
  uint16_t get_last_raw();
  void get_short_name(char * buffer, uint8_t buffer_size);
  uint8_t get_pin();

  Sensor(const char * short_name, float accum_rate,
        uint8_t analog_pin, uint16_t zero_adjust, float gain);

 private:
  // read_sensor should read a fresh, uniqe sensor reading every time it's
  //   called, with the minimal time spent.
  virtual uint16_t read_sensor() = 0;

  // data - config
  // todo: I could save space here if I make the sensors stateless
  char short_name[SENSOR_SHORT_NAME_LEN];
  float accum_rate;
  uint16_t zero_adjust;
  float gain;
  // data - state
  float avg_value;

 protected:
  uint8_t pin;        // used by implementation
  uint16_t last_raw;  // last output from read_sensor

  Sensor() = delete;  // require explicit construction
};

#endif  // SENSOR_HH_
