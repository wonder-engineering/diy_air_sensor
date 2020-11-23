// Copyright 2020 Brett M. Leedy
/*
  SensorState class

  This class represents the one "database" of context information
    for the DIY air sensor.  It is read and updated by various
    methods:

    * SensorMenu changes the settings
    * SensorArray inputs the number of sensors and their config
    * SensorArray updates the data values when we call sense()
    * LogFile reads values out and writes them to the logfile
    * AirSensorDisplay reads values out and displays them
*/

#ifndef SENSOR_STATE_HH
#define SENSOR_STATE_HH

#include "Arduino.h"
#include "Sensor.hh"
#include "LogFile.h"
#include "SensorSettings.hh"

enum SdState {
  kFileOk = 0,
  kFileFail,
};

struct SensorData {
  float value = 0.0;
  uint16_t raw=0;
};

struct DeviceStatus {
  char current_file_name[MAX_FILENAME_LEN];
  SdState file_status = kFileFail;
  uint8_t num_sensors = 0;
  SensorSettings settings;  // EEPROM Settings
};

struct SensorConfig {
  char shortname[SENSOR_SHORT_NAME_LEN] = "";
};

struct SensorInfo {
  SensorData data;      // initialized at first read
  SensorConfig config;  // read and written to EEPROM
};

// the one "database" class of state info
struct SensorState {
  public:
  SensorInfo sensor[SENSORSTATE_MAX_NUM_SENSORS];
  DeviceStatus device;
};

#endif
