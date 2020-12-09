// Copyright 2020 Brett M Leedy
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/AirSensorDisplay.hh"


using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;
using ::testing::NotNull;

TEST(AirSensorDisplay, display_data) {
  class InstrumentedAirSensorDisplay : public AirSensorDisplay {
   public:
    // void rotate_file() {AirSensorDisplay::rotate_file();}
    // char * get_file_name_ptr() {return current_name;}
    // void replace_lcd_interface(Sd_i * interface) {
    //   this->sd = interface;
    // }
    // void reset_first_run() {this->first_run = true;}  // for testing
    // bool re_init_sd() {return AirSensorDisplay::re_init_sd();}
  };
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
