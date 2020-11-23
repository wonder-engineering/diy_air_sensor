// Copyright 2020 Brett M. Leedy
/*
  DiyAirSensor

  This class wraps contains all of the functionality of the DIY Air Sensor
    and is responsible for it's configuration and execution order.
*/
#ifndef DIY_AIR_SENSOR_HH_
#define DIY_AIR_SENSOR_HH_


#include "Arduino.h"
#include "SensorArray.hh"
// #include "LogFile.h"  todo: fix up and restore
#include "GP2YSensor.hh"
#include "MQSensor.hh"
#include "AirSensorDisplay.hh"

#define SECONDS_PER_DAY      86400
#define SENSOR_ACCUM_RATE    0.1
#define DEFAULT_ZERO_ADJUST  0
#define DEFAULT_GAIN         1.0

class DiyAirSensor {
  public:
    DiyAirSensor();  // called once at start in diy_air_sensor.ino
    void loop();     // called continuously forever after start

  private:
    // Classes for main components
    SensorState       sensor_state;  // one "database" for stateful data
    // LogFile           * logfile;
    SensorArray       * sensors;     // container for all the sensors
    AirSensorDisplay  * sensor_display;
    uint32_t loop_number;
    uint32_t loop_start_millis;
};

  #endif
