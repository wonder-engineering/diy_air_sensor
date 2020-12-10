// Copyright 2020 Brett M Leedy
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/AirSensorDisplay.hh"


using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;
using ::testing::NotNull;
using ::testing::InSequence;


SensorState fake_sensor_state = {
  .sensor[0].data.value=0.0,
  .sensor[0].data.avg=0.0,
  .sensor[0].data.raw=0,
  .sensor[0].config.shortname="GAS0",
  .sensor[1].data.value=10.0,
  .sensor[1].data.avg=10.0,
  .sensor[1].data.raw=10,
  .sensor[1].config.shortname="GAS1",
  .sensor[2].data.value=100.0,
  .sensor[2].data.avg=100.0,
  .sensor[2].data.raw=100,
  .sensor[2].config.shortname="GAS2",
  .sensor[3].data.value=1000.0,
  .sensor[3].data.avg=1000.0,
  .sensor[3].data.raw=1000,
  .sensor[3].config.shortname="GAS3",
  .sensor[4].data.value=888.0,
  .sensor[4].data.avg=888.0,
  .sensor[4].data.raw=888,
  .sensor[4].config.shortname="GAS4",
  .sensor[5].data.value=0.0,
  .sensor[5].data.avg=500.0,
  .sensor[5].data.raw=0,
  .sensor[5].config.shortname="GAS5",
  .device.file_status=kFileOk,
  .device.num_sensors=6,
  .device.settings.data.sensor_thresholds[0]=100,
  .device.settings.data.sensor_thresholds[1]=100,
  .device.settings.data.sensor_thresholds[2]=100,
  .device.settings.data.sensor_thresholds[3]=100,
  .device.settings.data.sensor_thresholds[4]=100,
  .device.settings.data.sensor_thresholds[5]=100
  // other settings left as defaults
};


TEST(AirSensorDisplay, display_data) {
  class InstrumentedAirSensorDisplay : public AirSensorDisplay {
   public:
    MOCK_METHOD(void,
                checkForMenuButtons,
                (SensorState * sensor_state),
                (override));
    MOCK_METHOD(void,
                displaySensorColumns,
                (SensorState * sensor_state),
                (override));
    MOCK_METHOD(void,
                displaySensorThresholdWarnings,
                (SensorState * sensor_state),
                (override));
  } display;

  {
    InSequence s;
    // Expect these methods to be called in sequence
    EXPECT_CALL(display, checkForMenuButtons(_));
    EXPECT_CALL(display, displaySensorColumns(_));
    EXPECT_CALL(display, displaySensorThresholdWarnings(_));
  }
  display.display_data(&fake_sensor_state);


  // InstrumentedAirSensorDisplay logfile;

  // for (int i = 0; i<= 50000; i++) {
  //   logfile.rotate_file();
  //   int overall_string_length = strlen(logfile.get_file_name_ptr());
  //   // filename is less than 12 characters total (8.3 convention)
  //   // ASSERT_LT(overall_string_length, 13);
  //   int period_pos = 0;
  //   int num_periods = 0;
  //   for (int pp = 0; pp < overall_string_length; pp++) {
  //     if (logfile.get_file_name_ptr()[pp] == '.') {
  //           num_periods++;
  //           period_pos = pp;
  //       }
  //   }
  //   // before extension should be 8 or less characters
  //   ASSERT_LT(period_pos, 8);
  //   // extension should be 3 or less characters long to meet the standard
  //   ASSERT_LT((overall_string_length - period_pos), 5);
  //   // should only have one period
  //   ASSERT_LT(num_periods, 2);
  //   // extension should be exactly ".CSV"
  //   ASSERT_STREQ(logfile.get_file_name_ptr()+period_pos, ".CSV");
  // }
}
