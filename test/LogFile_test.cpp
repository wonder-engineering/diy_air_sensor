// Copyright 2014 http://switchdevice.com
// This example code is in the public domain.


#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "arduino-mock/Arduino.h"
#include "arduino-mock/Serial.h"
//#include "arduino-mock/SD.h"
//#include "arduino-mock/SPI.h"
//#include "arduino-mock/EEPROM.h"
//#include "arduino-mock/Wire.h"

//#define File file
#include "File_mock.h"

SDClass SD;

#include "../LogFile.h"
#include "../LogFile.cpp"

class MockLogFile : public LogFile {
	public:
		...
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
//  ArduinoMock* arduinoMock = arduinoMockInstance();
//  SerialMock* serialMock = serialMockInstance();
//	MockLogFile * logfile;

//	logfile = new MockLogFile();
//	EXPECT_CALL(logfile, re_init_sd()).Times(AtLeast(1));

//	logfile->open_line(1, 1);
//	logfile->close_line();
	// EXPECT_CALL(*arduinoMock, digitalRead(2))
  //  .WillOnce(Return(1));
  // EXPECT_CALL(*serialMock, println(1, 10));
  // EXPECT_CALL(, delay(1));
  //releaseSerialMock();
  //releaseArduinoMock();
}



