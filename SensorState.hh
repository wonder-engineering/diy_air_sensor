#ifndef SENSOR_STATE_HH
#define SENSOR_STATE_HH

#include "SensorArray.hh"

enum SdState {
  kFileOk = 0,
  kFileFail,
};

struct SensorData {
  float value = 0.0;
};

struct DeviceStatus {
  SdState file_status = kFileFail;
  uint8_t num_sensors = 0;
  uint16_t sampling_period_ms = 0;
  uint16_t log_every_n_loops = 10; // todo: make constants for defaults
  bool logging_enabled = false;
  bool is_alternate_config = false;
};

struct SensorConfig {
  uint16_t threshold;
  char shortname[SENSOR_SHORT_NAME_LEN] = "";
};

struct SensorInfo {
  SensorData data;      // initialized at first read
  SensorConfig config;  // read and written to EEPROM
};

// the one "database" class of state info
class SensorState {
  public:
  SensorInfo sensor[MAX_NUM_SENSORS];
  DeviceStatus device;
};

#endif
