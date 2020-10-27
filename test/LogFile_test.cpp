// Copyright 2020 Brett M Leedy

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Stub_Arduino.h"
#include "Stub_Serial.h"

#include "File_mock.h"


#include "../LogFile.h"
#include "../LogFile.cpp"



class MockLogFile : public LogFile {
	public:
		MOCK_METHOD(void, set_pinmode, (uint8_t pin, uint8_t flags), (override));
		MOCK_METHOD(uint16_t, get_highest_used_id, (), (override));
};

class MockSd_i : public Sd_i {
				public:
							MOCK_METHOD(bool, begin, (uint8_t csPin), (override));
};

using ::testing::AtLeast;

// Tests I would like to run:
//   * If SD fails on start, will any of my methods try to call it?
//   * Do I mark SD failed correctly
//   * Do I try to heal upon SD failure
//   * Am I honoring the cooldown period?
//   * Do I return a correct filename?
//   * Do my filenames comply with SD library naming conventions?
//   * Does open line open the correct file name?
//   * Do I discover the latest file name?
//   * Does File rotation work?
//   * Do I build proper CSV Files?
//   * Do I prevent closing a line before opening one?
//   * Do I prevent writing to an unopened line?
//   * Is my max filename length variable set too high?
//   * If my number of files grows to 10000, will I violate the naming conventions?
//   * Do I call delay anywhere?
//   * Do I call Serial.print() anywhere unnecessarily?
// https://github.com/google/googletest/blob/master/googlemock/docs/cheat_sheet.md
// * we handle un-initiated EEPROM correctly
// checksum works as expected
// each config update works as expected, and loading it from EEPROM works as expected.
//
//
//
//
using ::testing::Return;
using ::testing::_;
TEST(LogFile, Initialization_happy) {
	MockLogFile logfile;
  MockSd_i mock_sd;

	// Calling mocked SD begin() will return success
  ON_CALL(mock_sd, begin(_)).WillByDefault([this](uint8_t n){return true;});
	
	// construct the mock logfile
	logfile.replace_sd_interface(&mock_sd);

  // at construct, we:
	// * didn't drop into cooldown
	// * did init cooldown millis
	// Did set pinmode for pin 10 to output
	EXPECT_CALL(logfile, set_pinmode(10, OUTPUT));
	// Init the SD card with CS pin 10 (part of elec. design)
	EXPECT_CALL(mock_sd, begin(10));
	// * Did call get_highest_used_id
  EXPECT_CALL(logfile, get_highest_used_id());
	// * Assemble the correct logfile name from that ID
	// ASSERT_STREQ(str1,str2);
	// * return false
  bool init_error = logfile.re_init_sd();
	ASSERT_FALSE(init_error);
  bool sd_failure = logfile.get_sd_failure();
  ASSERT_FALSE(sd_failure);

}



