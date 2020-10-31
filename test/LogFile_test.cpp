// Copyright 2020 Brett M Leedy

#include "../LogFile.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

// SD interface mock
class MockSd_i : public Sd_i {
				public:
							MOCK_METHOD(bool, begin, (uint8_t csPin), (override));
							MOCK_METHOD(File, open, (const char * name, uint8_t mode), (override));
							MOCK_METHOD(void, end, (),(override));
};

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;

TEST(LogFile, File_Names_comply){
  LogFile logfile;

  for (int i = 0; i<= 50000; i++) {
	  logfile.rotate_file();
		int overall_string_length=strlen(logfile.get_file_name_ptr());
		// filename is less than 12 characters total (8.3 convention)
		ASSERT_LT(overall_string_length, 13);
		int period_pos = 0;
		int num_periods = 0;
		for (int pp = 0; pp < overall_string_length; pp++){
		  if (logfile.get_file_name_ptr()[pp] == '.') {
        num_periods++;
				period_pos=pp;
			}
		}
		// before extension should be 8 or less characters
		ASSERT_LT( period_pos, 8);
		// extension should be 3 or less characters long to meet the standard
		ASSERT_LT( (overall_string_length - period_pos), 5);
		// should only have one period
		ASSERT_LT(num_periods, 2);
		// extension should be exactly ".CSV"
		ASSERT_STREQ(logfile.get_file_name_ptr()+period_pos, ".CSV");
	}
}
//   Test whether SD init failures are handled correctly
TEST(LogFile, SD_init_Failure_Handling) {
	//// Mock Classes that need mocking
	// Mock LogFile methods we want to assert/manipulate			
  class MockLogFile : public LogFile {
	  public:
     MOCK_METHOD(uint16_t, get_highest_used_id, (), (override));
  };

	//// Instantiate mocked classes
	MockLogFile logfile;
  MockSd_i mock_sd;

	///// Override any mocked methods as needed
	// Calling mocked SD begin() will always return failure
  ON_CALL(mock_sd, begin(_)).WillByDefault([this](uint8_t n){return false;});

	//// Call any setup methods needed here
	logfile.replace_sd_interface(&mock_sd);
	logfile.reset_first_run();  //make the class think it's the first run again

  //// Reset any counters that might have been triggered in setup here
	Mock::VerifyAndClearExpectations(&logfile);
	Mock::VerifyAndClearExpectations(&mock_sd);

	//// Set expectations for mocks here
  // at construct, we:
	// Call the sd begin method, which will fail
	EXPECT_CALL(mock_sd, begin(_));
	// we will not call get_highest_used_id, because it will always
	// fail and give us wrong results.
  EXPECT_CALL(logfile, get_highest_used_id()).Times(0);


	// re_init_sd() should return true (signaling an error)
	ASSERT_TRUE(logfile.re_init_sd());
	
	// our failure state should be marked true
	ASSERT_TRUE(logfile.is_sd_failed());

}

//   Test whether SD failures cause re-init
TEST(LogFile, SD_heal_after_failure) {
	//// Mock Classes that need mocking
	// Mock LogFile methods we want to assert/manipulate			
  class MockLogFile : public LogFile {
	  public:
  };

	//// Instantiate mocked classes
	MockLogFile logfile;
  MockSd_i mock_sd;

	///// Override any mocked methods as needed
	// Calling mocked SD begin() will always return success (true)
  ON_CALL(mock_sd, begin(_)).WillByDefault([this](uint8_t n){return true;});
	int file_value = 0;
	File * file_ptr = (File *)&file_value; // failure to open evaluates to FALSE
	ON_CALL(mock_sd, open(_,_)).WillByDefault(Return(*file_ptr));

	//// Call any setup methods needed here
	logfile.replace_sd_interface(&mock_sd);

  //// Reset any counters that might have been triggered in setup here
	Mock::VerifyAndClearExpectations(&logfile);
	Mock::VerifyAndClearExpectations(&mock_sd);

	//// Set expectations for mocks here
  // at construct, we:
	// Call the sd begin method, which will fail
	EXPECT_CALL(mock_sd, open(_,_)).Times(1);

	//// Try to open a Line and should fail
	logfile.open_line(0,0);
  ASSERT_TRUE(logfile.is_sd_failed());

}

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
	ASSERT_STREQ("L-1.CSV",logfile.get_file_name_ptr());
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

TEST(LogFile, re_init_behaviors) {
	//// Mock Classes that need mocking
	// Mock LogFile methods we want to assert/manipulate			
  class MockLogFile : public LogFile {
	  public:
		  MOCK_METHOD(void, set_pinmode, (uint8_t pin, uint8_t flags), (override));
		  MOCK_METHOD(uint16_t, get_highest_used_id, (), (override));
		  MOCK_METHOD(uint16_t, get_millis, (), (override));
  };

	//// Instantiate mocked classes
	MockLogFile logfile;
  MockSd_i mock_sd;

	///// Override any mocked methods as needed
	// Calling mocked SD begin() will return success
  ON_CALL(mock_sd, begin(_)).WillByDefault([this](uint8_t n){return true;});

	//// Call any setup methods needed here
	// construct the mock logfile
	logfile.replace_sd_interface(&mock_sd);

	Mock::VerifyAndClearExpectations(&logfile);
	Mock::VerifyAndClearExpectations(&mock_sd);

	//// Set expectations for mocks here
	// Run 1:  Expect no calls to init methods if we're in cooldown
	EXPECT_CALL(logfile, get_millis()).Times(1).WillRepeatedly(Return(500));
	EXPECT_CALL(logfile, set_pinmode(10, OUTPUT)).Times(0);
	EXPECT_CALL(mock_sd, begin(10)).Times(0);
  EXPECT_CALL(logfile, get_highest_used_id()).Times(0);

	ASSERT_FALSE(logfile.re_init_sd());

	// Run 2:  Expect no calls to init methods after 1.5 seconds of cooldown
	Mock::VerifyAndClearExpectations(&logfile);
	Mock::VerifyAndClearExpectations(&mock_sd);
	EXPECT_CALL(logfile, get_millis()).Times(1).WillRepeatedly(Return(1500));
	EXPECT_CALL(logfile, set_pinmode(10, OUTPUT)).Times(0);
	EXPECT_CALL(mock_sd, begin(10)).Times(0);
  EXPECT_CALL(logfile, get_highest_used_id()).Times(0);

	ASSERT_FALSE(logfile.re_init_sd());
	
	// Run 3:  Expect calls to init methods after 6 seconds of cooldown
	Mock::VerifyAndClearExpectations(&logfile);
	Mock::VerifyAndClearExpectations(&mock_sd);
	EXPECT_CALL(logfile, get_millis()).Times(1).WillRepeatedly(Return(6000));
	EXPECT_CALL(logfile, set_pinmode(10, OUTPUT)).Times(1);
	EXPECT_CALL(mock_sd, begin(10)).Times(1);
  EXPECT_CALL(logfile, get_highest_used_id()).Times(1);

	ASSERT_FALSE(logfile.re_init_sd());
	

	// Run 5:  Expect no calls to init methods after 6.5 seconds of cooldown
	Mock::VerifyAndClearExpectations(&logfile);
	Mock::VerifyAndClearExpectations(&mock_sd);
	EXPECT_CALL(logfile, get_millis()).Times(1).WillRepeatedly(Return(6500));
	EXPECT_CALL(logfile, set_pinmode(10, OUTPUT)).Times(0);
	EXPECT_CALL(mock_sd, begin(10)).Times(0);
  EXPECT_CALL(logfile, get_highest_used_id()).Times(0);

	ASSERT_FALSE(logfile.re_init_sd());
	

	// Run 6:  Expect calls to init methods after 20 seconds of cooldown
	Mock::VerifyAndClearExpectations(&logfile);
	Mock::VerifyAndClearExpectations(&mock_sd);
	EXPECT_CALL(logfile, get_millis()).Times(1).WillRepeatedly(Return(20000));
	EXPECT_CALL(logfile, set_pinmode(10, OUTPUT)).Times(1);
	EXPECT_CALL(mock_sd, begin(10)).Times(1);
  EXPECT_CALL(logfile, get_highest_used_id()).Times(1);

	ASSERT_FALSE(logfile.re_init_sd());
	
	// Run 7:  Expect calls to init methods when wrapping
	Mock::VerifyAndClearExpectations(&logfile);
	Mock::VerifyAndClearExpectations(&mock_sd);
	EXPECT_CALL(logfile, get_millis()).Times(1).WillRepeatedly(Return(150));
	EXPECT_CALL(logfile, set_pinmode(10, OUTPUT)).Times(1);
	EXPECT_CALL(mock_sd, begin(10)).Times(1);
  EXPECT_CALL(logfile, get_highest_used_id()).Times(1);

	ASSERT_FALSE(logfile.re_init_sd());
}

