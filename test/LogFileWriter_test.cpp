// Copyright 2020 Brett M Leedy
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../log_file_writer/LogFileWriter.hh"

// SD interface mock
class MockSd_i : public Sd_i {
 public:
    MOCK_METHOD(bool, begin, (uint8_t csPin), (override));
    MOCK_METHOD(File, open, (const char * name, uint8_t mode), (override));
    MOCK_METHOD(void, end, (), (override));
    void rotate_file() {}
};

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;
using ::testing::NotNull;

TEST(LogFileWriter, File_Names_comply) {
  class InstrumentedLogFileWriter : public LogFileWriter {
   public:
    void rotate_file() {LogFileWriter::rotate_file();}
    char * get_file_name_ptr() {return current_name;}
    void replace_sd_interface(Sd_i * interface) {
      this->sd = interface;
    }
    void reset_first_run() {this->first_run = true;}  // for testing
    bool re_init_sd() {return LogFileWriter::re_init_sd();}
  };
  InstrumentedLogFileWriter logfile;

  for (int i = 0; i<= 50000; i++) {
    logfile.rotate_file();
    int overall_string_length = strlen(logfile.get_file_name_ptr());
    // filename is less than 12 characters total (8.3 convention)
    // ASSERT_LT(overall_string_length, 13);
    int period_pos = 0;
    int num_periods = 0;
    for (int pp = 0; pp < overall_string_length; pp++) {
      if (logfile.get_file_name_ptr()[pp] == '.') {
            num_periods++;
            period_pos = pp;
        }
    }
    // before extension should be 8 or less characters
    ASSERT_LT(period_pos, 8);
    // extension should be 3 or less characters long to meet the standard
    ASSERT_LT((overall_string_length - period_pos), 5);
    // should only have one period
    ASSERT_LT(num_periods, 2);
    // extension should be exactly ".CSV"
    ASSERT_STREQ(logfile.get_file_name_ptr()+period_pos, ".CSV");
  }
}


//   Test whether SD init failures are handled correctly
TEST(LogFileWriter, SD_init_Failure_Handling) {
  //// Mock Classes that need mocking
  // Mock LogFileWriter methods we want to assert/manipulate
  class MockLogFileWriter : public LogFileWriter {
   public:
    void rotate_file() override {LogFileWriter::rotate_file();}
    char * get_file_name_ptr() {return current_name;}
    void replace_sd_interface(Sd_i * interface) {
      this->sd = interface;
    }
    void reset_first_run() {this->first_run = true;}  // for testing
    bool re_init_sd() override {return LogFileWriter::re_init_sd();}
    bool is_sd_failed() override {return LogFileWriter::is_sd_failed();}

    MOCK_METHOD(uint16_t, get_highest_used_id, (), (override));
  };

  //// Instantiate mocked classes
  MockLogFileWriter logfile;
  MockSd_i mock_sd;

  ///// Override any mocked methods as needed
  // Calling mocked SD begin() will always return failure
  ON_CALL(mock_sd, begin(_)).WillByDefault([this](uint8_t n){return false;});

  //// Call any setup methods needed here
  logfile.replace_sd_interface(&mock_sd);
  logfile.reset_first_run();  // make the class think it's the first run again

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
  // todo: disabled for now because it's slow.
  // ASSERT_TRUE(logfile.is_sd_failed());
}

//   Test whether SD failures cause re-init
TEST(LogFileWriter, SD_heal_after_failure) {
  //// Mock Classes that need mocking
  // Mock LogFileWriter methods we want to assert/manipulate
  class MockLogFileWriter : public LogFileWriter {
   public:
    void rotate_file() {LogFileWriter::rotate_file();}
    char * get_file_name_ptr() {return current_name;}
    void replace_sd_interface(Sd_i * interface) {
      this->sd = interface;
    }
    void reset_first_run() {this->first_run = true;}  // for testing
    bool re_init_sd() {return LogFileWriter::re_init_sd();}
    bool is_sd_failed() {return LogFileWriter::is_sd_failed();}
    void open_line(uint32_t x, uint32_t y) {LogFileWriter::open_line(x, y);}
  };

  //// Instantiate mocked classes
  MockLogFileWriter logfile;
  MockSd_i mock_sd;

  ///// Override any mocked methods as needed
  // Calling mocked SD begin() will always return success (true)
  ON_CALL(mock_sd, begin(_)).WillByDefault([this](uint8_t n){return true;});
  int file_value = 0;
  File * file_ptr = (File *)&file_value;  // NOLINT
  ON_CALL(mock_sd, open(_, _)).WillByDefault(Return(*file_ptr));

  //// Call any setup methods needed here
  logfile.replace_sd_interface(&mock_sd);

  //// Reset any counters that might have been triggered in setup here
  Mock::VerifyAndClearExpectations(&logfile);
  Mock::VerifyAndClearExpectations(&mock_sd);

  //// Set expectations for mocks here
  // at construct, we:
  // Call the sd begin method, which will fail
  EXPECT_CALL(mock_sd, open(_, _)).Times(1);

  //// Try to open a Line and should fail
  logfile.open_line(0, 0);
  // todo: re-enable after re-enabling this part of the code
  // ASSERT_TRUE(logfile.is_sd_failed());
}

TEST(LogFileWriter, Happy_initialization_works) {
  //// Mock Classes that need mocking
  // Mock LogFileWriter methods we want to assert/manipulate
  class MockLogFileWriter : public LogFileWriter {
   public:
    void rotate_file() override {LogFileWriter::rotate_file();}
    char * get_file_name_ptr() {return current_name;}
    void replace_sd_interface(Sd_i * interface) {
      this->sd = interface;
    }
    void reset_first_run() {this->first_run = true;}  // for testing
    bool re_init_sd() override {return LogFileWriter::re_init_sd();}
    bool is_sd_failed() override {return LogFileWriter::is_sd_failed();}
    void open_line(uint32_t x, uint32_t y) override {
      LogFileWriter::open_line(x, y);
    }
   public:
      MOCK_METHOD(void, set_pinmode, (uint8_t pin, uint8_t flags), (override));
      MOCK_METHOD(uint16_t, get_highest_used_id, (), (override));
      MOCK_METHOD(bool, is_first_run, (), (override));
  };

  //// Instantiate mocked classes
  MockLogFileWriter logfile;
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
  ASSERT_STREQ("L-1.CSV", logfile.get_file_name_ptr());
  // no init error flagged
  ASSERT_FALSE(init_error);
  bool sd_failure = logfile.is_sd_failed();
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
// 1         500       yes             0       // too early
// 2         1500      yes             0       // still too early
// 3         6000      no              6000    // cooldown complete
// 4         6500      yes             6000    // too early after init
// 5         20000     no              20000   // cooldown complete
// 6         123       no              123     // wrapped, so run
//

TEST(LogFileWriter, re_init_behaviors) {
    //// Mock Classes that need mocking
    // Mock LogFileWriter methods we want to assert/manipulate
  class MockLogFileWriter : public LogFileWriter {
   public:
    void rotate_file() override {LogFileWriter::rotate_file();}
    char * get_file_name_ptr() {return current_name;}
    void replace_sd_interface(Sd_i * interface) {
      this->sd = interface;
    }
    void reset_first_run() {this->first_run = true;}  // for testing
    bool re_init_sd() override {return LogFileWriter::re_init_sd();}
    bool is_sd_failed() override {return LogFileWriter::is_sd_failed();}
    void open_line(uint32_t x, uint32_t y) override {
      LogFileWriter::open_line(x, y);
    }

   public:
    MOCK_METHOD(void, set_pinmode, (uint8_t pin, uint8_t flags),
                (override));
    MOCK_METHOD(uint16_t, get_highest_used_id, (), (override));
    MOCK_METHOD(uint16_t, get_millis, (), (override));
  };

  //// Instantiate mocked classes
  MockLogFileWriter logfile;
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


TEST(LogFileWriter, get_sd_failure) {
  //// Mock Classes that need mocking
  // Mock LogFileWriter methods we want to assert/manipulate
  class MockLogFileWriter : public LogFileWriter {
   public:
    void set_sd_failure(bool override_value) {
      this->sd_failure = override_value;
    }
    bool get_sd_failure() {return LogFileWriter::get_sd_failure();}
  };

  //// Instantiate mocked classes
  MockLogFileWriter logfile;

  logfile.set_sd_failure(true);
  ASSERT_TRUE(logfile.get_sd_failure());

  logfile.set_sd_failure(false);
  ASSERT_FALSE(logfile.get_sd_failure());
}

  class ListenForLineMockLogFileWriter : public LogFileWriter {
   public:
    MOCK_METHOD(ParsingState,
                update_state,
                (uint8_t, ParsingState),
                (override));
    MOCK_METHOD(bool,
                serial_bytes_available,
                (),
                (override));
    MOCK_METHOD(uint8_t,
                read_serial_byte,
                (),
                (override));
    MOCK_METHOD(void,
                open_line,
                (uint32_t, uint32_t),
                (override));
    MOCK_METHOD(void,
                close_line,
                (),
                (override));
    MOCK_METHOD(void,
                set_new_filename,
                (uint16_t file_number),
                (override));
    ParsingState fortest_get_parser_state() {
      return this->parserState;
    }
    void override_host_file_number(uint16_t new_id) {
      this->host_file_id = new_id;
    }
    void override_file(File * oFile) {this->file = oFile;}
  };



// actions we espect when we're in init state
TEST(LogFileWriter, listen_for_line_initState) {
  // Instantiate mocked classes
  ListenForLineMockLogFileWriter logfile;

  // Set exepectations
  // always call read byte, then update state
  {
    testing::InSequence s;
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(true));
    EXPECT_CALL(logfile,
      read_serial_byte()).Times(1).WillOnce(
        Return(HEADER_START));
    EXPECT_CALL(logfile,
      update_state(_, _)).Times(1).WillOnce(
        Return(initState));
    // signal no more bytes or we'll loop forever
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(false));
  }
  EXPECT_CALL(logfile, open_line(_, _)).Times(0);
  EXPECT_CALL(logfile, close_line()).Times(0);

  logfile.listen_for_line();
  ASSERT_EQ(logfile.fortest_get_parser_state(), initState);
}

// actions we expect when we're in header received state
TEST(LogFileWriter, listen_for_line_headerReceivedState) {
  // Instantiate mocked classes
  ListenForLineMockLogFileWriter logfile;

  // Set exepectations
  // always call read byte, then update state
  {
    testing::InSequence s;
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(true));
    EXPECT_CALL(logfile,
      read_serial_byte()).Times(1).WillOnce(
        Return(HEADER_START));
    EXPECT_CALL(logfile,
      update_state(_, _)).Times(1).WillOnce(
        Return(headerReceivedState));
    // signal no more bytes or we'll loop forever
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(false));
  }
  EXPECT_CALL(logfile, open_line(_, _)).Times(0);
  EXPECT_CALL(logfile, close_line()).Times(0);

  logfile.listen_for_line();
  ASSERT_EQ(logfile.fortest_get_parser_state(), headerReceivedState);
}

// actions we expect when we're in delimiterReceivedState
TEST(LogFileWriter, listen_for_line_delimiterReceivedState) {
  // Instantiate mocked classes
  ListenForLineMockLogFileWriter logfile;

  // Set exepectations
  // always call read byte, then update state
  {
    testing::InSequence s;
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(true));
    EXPECT_CALL(logfile,
      read_serial_byte()).Times(1).WillOnce(
        Return(HEADER_START));
    EXPECT_CALL(logfile,
      update_state(_, _)).Times(1).WillOnce(
        Return(delimiterReceivedState));
    // signal no more bytes or we'll loop forever
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(false));
  }
  EXPECT_CALL(logfile, open_line(_, _)).Times(0);
  EXPECT_CALL(logfile, close_line()).Times(0);

  logfile.listen_for_line();
  ASSERT_EQ(logfile.fortest_get_parser_state(), delimiterReceivedState);
}

// actions we expect when we're in delimiterReceivedState
TEST(LogFileWriter, listen_for_line_fileIdReceivedState_notNew) {
  // Instantiate mocked classes
  ListenForLineMockLogFileWriter logfile;

  // set file number
  const uint8_t fake_file_number = 2;
  logfile.override_host_file_number(fake_file_number);

  // Set exepectations
  // always call read byte, then update state
  {
    testing::InSequence s;
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(true));
    // byte is the same as existing file number
    EXPECT_CALL(logfile,
      read_serial_byte()).Times(1).WillOnce(
        Return(fake_file_number));
    EXPECT_CALL(logfile,
      update_state(_, _)).Times(1).WillOnce(
        Return(fileIdReceivedState));
    // signal no more bytes or we'll loop forever
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(false));
  }
  // Should open the line at this point
  EXPECT_CALL(logfile, open_line(_, _)).Times(1);
  EXPECT_CALL(logfile, close_line()).Times(0);
  EXPECT_CALL(logfile, set_new_filename(_)).Times(0);

  logfile.listen_for_line();
  ASSERT_EQ(logfile.fortest_get_parser_state(), fileIdReceivedState);
}

// actions we expect when we're in ileIdReceivedState and get a new
//   file ID
TEST(LogFileWriter, listen_for_line_fileIdReceivedState_newFile) {
  // Instantiate mocked classes
  ListenForLineMockLogFileWriter logfile;

  // set file number
  const uint8_t fake_file_number = 2;
  logfile.override_host_file_number(fake_file_number);

  // Set exepectations
  // always call read byte, then update state
  {
    testing::InSequence s;
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(true));
    // byte is the same as existing file number
    EXPECT_CALL(logfile,
      read_serial_byte()).Times(1).WillOnce(
        Return(fake_file_number+1));
    EXPECT_CALL(logfile,
      update_state(_, _)).Times(1).WillOnce(
        Return(fileIdReceivedState));
    // signal no more bytes or we'll loop forever
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(false));
  }
  // Should open the line at this point
  EXPECT_CALL(logfile, open_line(_, _)).Times(1);
  EXPECT_CALL(logfile, close_line()).Times(0);
  EXPECT_CALL(logfile, set_new_filename(_)).Times(1);

  logfile.listen_for_line();
  ASSERT_EQ(logfile.fortest_get_parser_state(), fileIdReceivedState);
}

// actions we expect when we're in writingBodyState
TEST(LogFileWriter, listen_for_line_writingBodyState) {
  class MockFile : public File {
   public:
    MOCK_METHOD(void, write, (uint8_t), (override));
  } mock_file;

  // Instantiate mocked classes
  ListenForLineMockLogFileWriter logfile;
  logfile.override_file(&mock_file);

  // set file number
  const uint8_t fake_byte = 'B';

  // Set exepectations
  // always call read byte, then update state
  {
    testing::InSequence s;
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(true));
    // byte is the same as existing file number
    EXPECT_CALL(logfile,
      read_serial_byte()).Times(1).WillOnce(
        Return(fake_byte));
    EXPECT_CALL(logfile,
      update_state(_, _)).Times(1).WillOnce(
        Return(writingBodyState));
    // signal no more bytes or we'll loop forever
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(false));
  }
  // Should open the line at this point
  EXPECT_CALL(logfile, open_line(_, _)).Times(0);
  EXPECT_CALL(logfile, close_line()).Times(0);
  EXPECT_CALL(logfile, set_new_filename(_)).Times(0);
  EXPECT_CALL(mock_file, write(fake_byte)).Times(1);

  logfile.listen_for_line();
  ASSERT_EQ(logfile.fortest_get_parser_state(), writingBodyState);
}

// actions we expect when we're in protocolFailureState
TEST(LogFileWriter, listen_for_line_protocolFailureState) {
  class MockFile : public File {
   public:
    MOCK_METHOD(void, write, (uint8_t), (override));
    MOCK_METHOD(void, print, (const char[]), (override));
  } mock_file;

  // Instantiate mocked classes
  ListenForLineMockLogFileWriter logfile;
  logfile.override_file(&mock_file);

  // set file number
  const uint8_t fake_byte = 'B';

  // Set exepectations
  // always call read byte, then update state
  {
    testing::InSequence s;
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(true));
    // byte is the same as existing file number
    EXPECT_CALL(logfile,
      read_serial_byte()).Times(1).WillOnce(
        Return(fake_byte));
    EXPECT_CALL(logfile,
      update_state(_, _)).Times(1).WillOnce(
        Return(protocolFailureState));
    // signal no more bytes or we'll loop forever
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(false));
  }

  // Should open the line at this point
  EXPECT_CALL(logfile, open_line(_, _)).Times(0);
  EXPECT_CALL(logfile, close_line()).Times(1);
  EXPECT_CALL(logfile, set_new_filename(_)).Times(0);
  EXPECT_CALL(mock_file, write(_)).Times(0);
  EXPECT_CALL(mock_file, print(_)).Times(1);

  logfile.listen_for_line();
  ASSERT_EQ(logfile.fortest_get_parser_state(), protocolFailureState);
}

// actions we expect when we're in endlineReceivedState
TEST(LogFileWriter, listen_for_line_endlineReceivedState) {
  class MockFile : public File {
   public:
    MOCK_METHOD(void, write, (uint8_t), (override));
    MOCK_METHOD(void, print, (const char[]), (override));
  } mock_file;

  // Instantiate mocked classes
  ListenForLineMockLogFileWriter logfile;
  logfile.override_file(&mock_file);

  // set file number
  const uint8_t fake_byte = 'B';

  // Set exepectations
  // always call read byte, then update state
  {
    testing::InSequence s;
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(true));
    // byte is the same as existing file number
    EXPECT_CALL(logfile,
      read_serial_byte()).Times(1).WillOnce(
        Return(fake_byte));
    EXPECT_CALL(logfile,
      update_state(_, _)).Times(1).WillOnce(
        Return(endlineReceivedState));
    // signal no more bytes or we'll loop forever
    EXPECT_CALL(logfile,
      serial_bytes_available()).Times(1).WillOnce(
        Return(false));
  }

  // Should open the line at this point
  EXPECT_CALL(logfile, open_line(_, _)).Times(0);
  EXPECT_CALL(logfile, close_line()).Times(1);
  EXPECT_CALL(logfile, set_new_filename(_)).Times(0);
  EXPECT_CALL(mock_file, write(_)).Times(0);
  EXPECT_CALL(mock_file, print(_)).Times(0);

  logfile.listen_for_line();
  ASSERT_EQ(logfile.fortest_get_parser_state(), endlineReceivedState);
}


// Test for the state machine
TEST(LogFileWriter, update_state) {
  class MockLogFileWriter : public LogFileWriter {
   public:
    ParsingState test_update_state(uint8_t byte, ParsingState oldState) {
      return LogFileWriter::update_state(byte, oldState);
    }
    uint16_t get_parser_row_bytes() {return this->parserRowBytes;}
  } logfile;

  ParsingState current_state = initState;

  // Stay in init state even if other random characters happen
  for (uint8_t ascii = 0;
        ascii < 255;
        ascii++) {
    if (ascii == HEADER_START)
      continue;
    current_state = logfile.test_update_state(ascii, current_state);
    ASSERT_EQ(current_state, initState);
  }

  // move to header received state and stay there no matter how many
  // times you receive the header char
  for (uint8_t i = 0; i < 5; i++) {
    current_state = logfile.test_update_state(HEADER_START, current_state);
    ASSERT_EQ(current_state, headerReceivedState);
  }

  // send a non-header byte - should transition to fileIdReceivedState
  current_state = logfile.test_update_state(2, current_state);
  ASSERT_EQ(current_state, fileIdReceivedState);

  // now send everything but START_TEXT and should signal a failure
  // Stay in init state even if other random characters happen
  for (uint8_t ascii = 0;
        ascii < 255;
        ascii++) {
    if (ascii == START_TEXT)
      continue;
    current_state = logfile.test_update_state(ascii, fileIdReceivedState);
    ASSERT_EQ(current_state, protocolFailureState);
  }

  // while in fileIdReceivedState, move to delimiterReceivedState
  //  when delimiter is received
  current_state = fileIdReceivedState;
  current_state = logfile.test_update_state(START_TEXT, current_state);
  ASSERT_EQ(current_state, delimiterReceivedState);


  // in delimiter received state, we already have the first byte of the body
  // should reset bytes in row at this point
  // todo: check for bad characters here like we check in writing body
  current_state = delimiterReceivedState;
  current_state = logfile.test_update_state('A', current_state);
  ASSERT_EQ(current_state, writingBodyState);
  ASSERT_EQ(logfile.get_parser_row_bytes(), 0);

  // test writing the body.
  // stays in writing body state for valid characters
  current_state = writingBodyState;
  const char valid_test_body[] =  "12345678910,"
                                  "ABCDEFGHIGKLMNOPQRSTUVWXYZ,"
                                  "abcdefghijklmnopqrstuvwxyz,"
                                  " _!@#$%^&*()_+,8,|{}-=<>?,./,"
                                  "345,success";
  uint16_t test_body_len = strlen(valid_test_body);
  for (int i=0; i < test_body_len; i++) {
    current_state = logfile.test_update_state(valid_test_body[i],
                                              current_state);
    ASSERT_EQ(current_state, writingBodyState);
    // counts all of the characters written to the body
    ASSERT_EQ(logfile.get_parser_row_bytes(), i+1);
  }

  // test that we fail for out-of-range characters
  current_state = writingBodyState;
  current_state = logfile.test_update_state(0xFF, current_state);
  ASSERT_EQ(current_state, protocolFailureState);

  // test that we transiton to endline when we get the end char
  current_state = writingBodyState;
  current_state = logfile.test_update_state(END_TEXT, current_state);
  ASSERT_EQ(current_state, endlineReceivedState);

  // run up the body size (as many as the max bytes+1) and make sure we signal
  //  protocol failure
  current_state = writingBodyState;
  for (int i = 0; i <= MAX_BYTES_PER_ROW; i++) {
    current_state = logfile.test_update_state('G', current_state);
    if (current_state == protocolFailureState)
      break;  // this is success, no need to continue
  }
  ASSERT_EQ(current_state, protocolFailureState);
  ASSERT_EQ(logfile.get_parser_row_bytes(), MAX_BYTES_PER_ROW+1);

  // test that we automatically switch out of protocol failure state
  //   in every case.
  for (uint8_t ascii = 0;
        ascii < 255;
        ascii++) {
    current_state = logfile.test_update_state(ascii, protocolFailureState);
    ASSERT_NE(current_state, protocolFailureState);

    // in one special case, check that it transitions straight to
    //   header received
    if (ascii == HEADER_START)
      ASSERT_EQ(current_state, headerReceivedState);
  }
}


// Test for the state machine
TEST(LogFileWriter, get_file_name) {
  class MockLogFileWriter : public LogFileWriter {
   public:
    void test_get_file_name(char * buffer, uint8_t max_size) {
      LogFileWriter::get_file_name(buffer, max_size);
    }
  } logfile;

  // should copy the filename (default filename assumed)
  char buffer[MAX_FILENAME_LEN];
  logfile.test_get_file_name(buffer, MAX_FILENAME_LEN);
  ASSERT_STREQ(buffer, "L-1.CSV");

  // should crop the file name if the max size is too small
  char newbuffer[MAX_FILENAME_LEN];
  logfile.test_get_file_name(newbuffer, 3);
  ASSERT_STREQ(newbuffer, "L-1");
}
