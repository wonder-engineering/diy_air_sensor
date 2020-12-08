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
#include "LogFileSender.hh"

#define SECONDS_PER_DAY      86400
#define SENSOR_ACCUM_RATE    0.1
#define DEFAULT_ZERO_ADJUST  0
#define DEFAULT_GAIN         1.0
#define SD_FORMAT_VERSION    1

class DiyAirSensor {
 public:
    explicit DiyAirSensor(bool do_init = true);
    void initializeSensor();
    void loop();  // called continuously forever
    void log_sd(uint32_t id, uint32_t timestamp);

 protected:
    // Classes for main components
    SensorState       sensor_state;  // one "database" for stateful data
    LogFileSender     * logfile     = NULL;
    SensorArray       * sensors        = NULL;
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

#endif  // SRC_DIYAIRSENSOR_HH_
