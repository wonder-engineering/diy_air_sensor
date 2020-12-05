// Copyright 2020 Brett M. Leedy

/*
  LogFileWriter class


  Log Lines Protocol:
  0xFF 0xFF 0xFF              |   3 bytes header
  uint8_t                     |   Session File ID (starts at zero at powerup)
  '|'                         |   delimiter
  <arbitrary amount of data>  |   body data to be written
  '\n'                        |   endline delimiter

*/

// todo: rx indicator with LED_BUILTIN

#ifndef SRC_LOGFILEWRITER_H_
#define SRC_LOGFILEWRITER_H_

#ifndef IN_TESTING  // we will mock libs for tesing
  #include <Arduino.h>
  #include <SoftwareSerial.h>
  #include <SPI.h>
#else
  #include <stdlib.h>
  #include "../test/Stub_Arduino.h"
  #include "../test/Stub_Serial.h"
  #include "../test/File_mock.h"
#endif
#include "Sd_i.hh"
#include "LogSerialInterface.hh"

#define MAX_FILENAME_LEN 12  // maximum of 8 characters for FAT
const char log_file_name_base[] = "L";
#define LOGFILE_EXTENSION "CSV"
#define SD_COOLDOWN_LENGTH 5000  // milliseconds

#define HEADER_START  0x01   // ASCII Start Of Header
#define START_TEXT    0x02   // ASCII Start of Text
#define END_TEXT      0x03   // ASCII End of Text
#define SOFTWARESERIAL_BAUD  9600
#define SOFTWARESERIAL_RX_PIN  2
#define SOFTWARESERIAL_TX_PIN  3

// todo: for test
#define ASCII_BODY_MAX 126
#define ASCII_BODY_MIN  32
#define MAX_BYTES_PER_ROW 200



enum ParsingState {
  initState = 0,
  headerReceivedState,
  fileIdReceivedState,
  delimiterReceivedState,
  writingBodyState,
  endlineReceivedState,
  protocolFailureState
};

class LogFileWriter {
 public:
  LogFileWriter();
  void listen_for_line();

 private:
  virtual uint16_t get_highest_used_id();
  virtual void override_file_number(uint16_t new_id);
  virtual bool begin_sd();
  virtual void set_pinmode(uint8_t pin, uint8_t flags);
  virtual void rotate_file();
  virtual void open_line(uint32_t id, uint32_t timestamp);
  virtual void close_line();
  virtual void get_file_name(char * buffer, uint8_t max_size);
  virtual bool re_init_sd();
  virtual bool is_sd_failed();
  ParsingState update_state(uint8_t byte, ParsingState oldstate);
  void set_new_filename(uint16_t file_number);
  bool is_valid_body_character(uint8_t character);

  bool get_sd_failure() {return this->sd_failure;}
  virtual bool is_first_run() {
    bool rv = this->first_run;
    this->first_run = false;
    return rv;
  }
  virtual uint16_t get_millis() {return millis();}
  void reset_first_run() {this->first_run = true;}  // for testing
  void replace_sd_interface(Sd_i * interface);
  char current_name[MAX_FILENAME_LEN];
  uint16_t highest_used_id = 0;
  uint16_t host_file_id = 0;
  bool sd_failure = true;
  uint16_t file_failure_count = 0;
  uint16_t sd_failure_count = 0;
  uint16_t cooldown_start_millis = 0;
  bool first_run = true;
  File file;
  Sd_i * sd = NULL;
  SoftwareSerial * serialPort;
  ParsingState parserState;  // stateful side-effect for parsing
  uint16_t parserRowBytes = 0;
};

#endif  // SRC_LOGFILE_H_
