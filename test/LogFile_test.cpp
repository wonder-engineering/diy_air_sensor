// Copyright 2020 Brett M Leedy

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Stub_Arduino.h"
#include "Stub_Serial.h"

#include "File_mock.h"

SDClass SD;

#include "../LogFile.h"
#include "../LogFile.cpp"

class MockLogFile : public LogFile {
	public:
		MOCK_METHOD(bool, re_init_sd, (), (override));
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
using ::testing::Return;
TEST(testsuite, testname) {
	MockLogFile * logfile;

	logfile = new MockLogFile();

	EXPECT_CALL(*logfile, re_init_sd()).Times(AtLeast(1));
  logfile->re_init_sd();
	delete logfile;
}



