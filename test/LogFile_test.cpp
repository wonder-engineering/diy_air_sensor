// Copyright 2020 Brett M Leedy

#include "../LogFile.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

// SD interface mock
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



using ::testing::Return;
using ::testing::_;
TEST(LogFile, Happy_initialization_works) {
	//// Mock Classes that need mocking
	// Mock LogFile methods we want to assert/manipulate			
  class MockLogFile : public LogFile {
	  public:
		  MOCK_METHOD(void, set_pinmode, (uint8_t pin, uint8_t flags), (override));
		  MOCK_METHOD(uint16_t, get_highest_used_id, (), (override));
		  MOCK_METHOD(bool, is_first_run, (), (override));
  };

	//// Instantiate mocked classes
	MockLogFile logfile;
  MockSd_i mock_sd;

	///// Override any mocked methods as needed
	// Calling mocked SD begin() will return success
  ON_CALL(mock_sd, begin(_)).WillByDefault([this](uint8_t n){return true;});
	// Will always treat as first run
  ON_CALL(logfile, is_first_run).WillByDefault([this](){return true;});

	//// Call any setup methods needed here
	// construct the mock logfile
	logfile.replace_sd_interface(&mock_sd);

	//// Set expectations for mocks here
  // at construct, we:
	// Didn't drop into cooldown, so the following happens
	// Did set pinmode for pin 10 to output
	EXPECT_CALL(logfile, set_pinmode(10, OUTPUT));
	// Init the SD card with CS pin 10 (part of elec. design)
	EXPECT_CALL(mock_sd, begin(10));
	// * Did call get_highest_used_id
  EXPECT_CALL(logfile, get_highest_used_id());

	//// Call actions and make assertions
	// * return false
  bool init_error = logfile.re_init_sd();
	// * Assemble the correct logfile name from that ID
	ASSERT_STREQ("LOG-1.CSV",logfile.get_file_name_ptr());
	// no init error flagged
	ASSERT_FALSE(init_error);
  bool sd_failure = logfile.get_sd_failure();
	// no sd failure flagge
  ASSERT_FALSE(sd_failure);

}


// re-init called 5 times:
// * increments the logfile
// * resets failure state
// * honors the cooldown
// * wraps the cooldown
//
// run       millis    cooldown skip?  cooldown start after?
// 0(const)  0         no              0       // constructor
// 1         500       yes             0       // too early after construct
// 2         1500      yes             0       // still too early after construct
// 3         6000      no              6000    // cooldown complete
// 4         6500      yes             6000    // too early after init
// 5         20000     no              20000   // cooldown complete
// 6         123       no              123     // wrapped, so run
//
/*
TEST(LogFile, re_init_behaviors) {
	//// Mock Classes that need mocking
	// Mock LogFile methods we want to assert/manipulate			
  class MockLogFile : public LogFile {
	  public:
		  MOCK_METHOD(void, set_pinmode, (uint8_t pin, uint8_t flags), (override));
		  MOCK_METHOD(uint16_t, get_highest_used_id, (), (override));
  };

	//// Instantiate mocked classes
	MockLogFile logfile;
  MockSd_i mock_sd;

	///// Override any mocked methods as needed
	// Calling mocked SD begin() will return success
  ON_CALL(mock_sd, begin(_)).WillByDefault([this](uint8_t n){return true;});
	// Will always treat as first run
  ON_CALL(logfile, is_first_run).WillByDefault([this](){return true;});

	//// Call any setup methods needed here
	// construct the mock logfile
	logfile.replace_sd_interface(&mock_sd);

	//// Set expectations for mocks here
  // at construct, we:
	// Didn't drop into cooldown, so the following happens
	// Did set pinmode for pin 10 to output
	EXPECT_CALL(logfile, set_pinmode(10, OUTPUT));
	// Init the SD card with CS pin 10 (part of elec. design)
	EXPECT_CALL(mock_sd, begin(10));
	// * Did call get_highest_used_id
  EXPECT_CALL(logfile, get_highest_used_id());

	//// Call actions and make assertions
	// * return false
  bool init_error = logfile.re_init_sd();
	// * Assemble the correct logfile name from that ID
	ASSERT_STREQ("LOG-1.CSV",logfile.get_file_name_ptr());
	// no init error flagged
	ASSERT_FALSE(init_error);
  bool sd_failure = logfile.get_sd_failure();
	// no sd failure flagge
  ASSERT_FALSE(sd_failure);

}
*/
