// Copyright 2020 Brett M. Leedy
/*
  DiyAirSensor

  This class wraps contains all of the functionality of the DIY Air Sensor
    and is responsible for it's configuration and execution order.
*/
#ifndef DIY_AIR_SENSOR_HH_
#define DIY_AIR_SENSOR_HH_

// todo: when IN_TEST is defined put gmock mocked classes in their place
#ifndef IN_TESTING
#include "Arduino.h"
#else
#include "test/Stub_Serial.h"
#endif

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
    DiyAirSensor(Serial_ * serial, bool do_init=false);  // called once at start in diy_air_sensor.ino
    void init(Serial_ *  serial);
    void loop(Serial_ * serial);     // called continuously forever after start

  protected:
    // Classes for main components
    SensorState       sensor_state;  // one "database" for stateful data
    // LogFile           * logfile     = NULL;
    SensorArray       * sensors        = NULL;     // container for all the sensors
    AirSensorDisplay  * sensor_display = NULL;
    uint32_t loop_number;
    uint32_t loop_start_millis;
    virtual void addSensors(SensorArray * array);
    virtual void initPins();
    virtual void updateConfigsFromSensor();
    virtual uint32_t getMillis();
    virtual void waitForSamplingPeriodEnd();
    bool initialized = false;
};

  #endif
