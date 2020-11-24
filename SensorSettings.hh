// Copyright 2020 Brett M. Leedy
/*
  SensorSettings class

  This class contains the setting data for the sensor.  On
    construction, it reads any existing settings from the EEPROM,
    validates them, and stores them.  This is the only persistent
    storage for the sensor.

    Change this class if you want to add more persistent data
    to the sensor (often when you add a new menu item).
*/

#ifndef SENSOR_SETTINGS_HH
#define SENSOR_SETTINGS_HH

#define DEFAULT_LOOP_PERIOD_MILLIS 2000  // every one second
#define DEFAULT_LOG_EVERY_N_LOOPS    10  // every ten seconds
#define SENSORSTATE_MAX_NUM_SENSORS 6

#ifdef IN_TESTING
#include "test/Stub_Arduino.h"
#include "test/Stub_Serial.h"
#else
#include "Arduino.h"
#endif


struct SettingsType {
  uint16_t sensor_thresholds[SENSORSTATE_MAX_NUM_SENSORS];
  uint16_t sampling_period_ms = DEFAULT_LOOP_PERIOD_MILLIS;
  uint16_t log_every_n_loops = DEFAULT_LOG_EVERY_N_LOOPS;
  uint16_t file_number = 0;
  bool log_raw = false;
  bool backlight = true;
  bool logging = true;
  bool display_raw = false;
  bool alt_sensor_config = false;
  uint32_t checksum = 0;
};


class SensorSettings {
 public:
  SensorSettings();
  virtual void commit();
  SettingsType data;

 private:
  void dump();
  void store_checksum();
  bool check();
  virtual void load_settings_from_eeprom(int idx, SettingsType * myconfig);
  uint32_t calc_checksum();

};

#endif
