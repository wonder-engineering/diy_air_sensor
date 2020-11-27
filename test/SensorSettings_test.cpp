// Copyright 2020 Brett M Leedy


#include "gtest/gtest.h"
#include "gmock/gmock.h"


#include "../SensorSettings.hh"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;
using ::testing::A;
using ::testing::Gt;
using ::testing::Lt;




#define HIGH_CHECKSUM       23976
#define SATURATED_CHECKSUM 0xFFFF
#define SANE_SAMPLING_PERIOD_MS 10000
#define SANE_LOG_EVERY_N_LOOPS  10
#define SANE_FILE_NUMBER  123
#define MEMORY_PADDING_MAX 3

// changes break checksum
// checksum covers all values
// once committed, the checsum succeeds
TEST(SensorSettings, behaviors) {

  class InstrumentedSensorSettings : public SensorSettings {
    public:
      bool check(){return SensorSettings::check();}
      void store_checksum(){SensorSettings::store_checksum();}
  };

  InstrumentedSensorSettings settings;
  int settings_overall_size = sizeof(settings.data);
  int settings_tested_size = 0;  // amount of the settings type that's tested
  for (int i = 0; i < SENSORSTATE_MAX_NUM_SENSORS; i++) {
	  settings.data.sensor_thresholds[i] = 5 * i;
  }
  settings_tested_size += sizeof(settings.data.sensor_thresholds);
  settings.data.sampling_period_ms = SANE_SAMPLING_PERIOD_MS;
  settings_tested_size += sizeof(settings.data.sampling_period_ms);
  settings.data.log_every_n_loops = SANE_LOG_EVERY_N_LOOPS;
  settings_tested_size += sizeof(settings.data.log_every_n_loops);
  settings.data.file_number = SANE_FILE_NUMBER;
  settings_tested_size += sizeof(settings.data.file_number);
  settings.data.log_raw = false;
  settings_tested_size += sizeof(settings.data.log_raw);
  settings.data.backlight = true;
  settings_tested_size += sizeof(settings.data.backlight);
  settings.data.logging = true;
  settings_tested_size += sizeof(settings.data.logging);
  settings.data.display_raw = false;
  settings_tested_size += sizeof(settings.data.display_raw);
  settings.data.alt_sensor_config = false;
  settings_tested_size += sizeof(settings.data.alt_sensor_config);
  settings_tested_size += sizeof(settings.data.checksum);

  // Assert that this test initted (and will test) all fields
  // in the data structure
  ASSERT_GE(settings_tested_size+MEMORY_PADDING_MAX, settings_overall_size);


  // Checksum edge cases
  settings.data.checksum = 0;  // arbitrary zero checksum
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.data.checksum = HIGH_CHECKSUM;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.data.checksum = SATURATED_CHECKSUM;
  ASSERT_FALSE(settings.check());

  // changing values in any field breaks checksum
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.data.sampling_period_ms = SANE_SAMPLING_PERIOD_MS + 1;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.data.log_every_n_loops = SANE_LOG_EVERY_N_LOOPS + 10;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.data.file_number = SANE_FILE_NUMBER * 0;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.data.sensor_thresholds[2] = settings.data.sensor_thresholds[2] + 5;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.data.log_raw = !settings.data.log_raw;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.data.backlight = !settings.data.backlight;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.data.logging = !settings.data.logging;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.data.alt_sensor_config = !settings.data.alt_sensor_config;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum

  settings_tested_size += sizeof(settings.data.checksum);
}
