// Copyright 2020 Brett M. Leedy
/*
  DiyAirSensor

  This class wraps contains all of the functionality of the DIY Air Sensor
    and is responsible for it's configuration and execution order.
*/
#ifndef SRC_DIYAIRSENSOR_HH_
#define SRC_DIYAIRSENSOR_HH_

// todo: when IN_TEST is defined put gmock mocked classes in their place
#ifndef IN_TESTING
#include "Arduino.h"
#else
#include "../test/Stub_Serial.h"
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
    explicit DiyAirSensor(HardwareSerial * serial, bool do_init = false);
    void init(HardwareSerial *  serial);
    void loop(HardwareSerial * serial,  // called continuously forever
    long unsigned int(&)());  // NOLINT

 protected:
    // Classes for main components
    SensorState       sensor_state;  // one "database" for stateful data
    // LogFile           * logfile     = NULL;
    SensorArray       * sensors        = NULL;
    AirSensorDisplay  * sensor_display = NULL;
    uint32_t loop_number;
    uint32_t loop_start_millis;
    virtual void addSensors(SensorArray * array);
    virtual void initPins();
    virtual void updateConfigsFromSensor();
    virtual uint32_t getMillis(long unsigned int(&millisFunc)());  // NOLINT
    virtual void waitForSamplingPeriodEnd(
       long unsigned int(&millisFunc)());  // NOLINT
    bool initialized = false;
};

#endif  // SRC_DIYAIRSENSOR_HH_
