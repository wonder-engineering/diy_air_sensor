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

// #define MAX_FILENAME_LEN 12  // maximum of 8 characters for FAT
// const char log_file_name_base[] = "L";
// #define LOGFILE_EXTENSION "CSV"
// #define SD_COOLDOWN_LENGTH 5000  // milliseconds
enum ParsingState {
  initState = 0,
  headerReceivedState,
  fileIdReceivedState,
  delimiterReceivedState,
  writingBodyState,
  endlineRecievedState,
  protocolFailureState
}

class LogFileWriter {
 public:
  LogFileWriter();
  bool listen_for_line();

 private:
  virtual uint16_t get_highest_used_id();
  virtual void override_file_number(uint16_t new_id);
  virtual bool begin_sd();
  virtual void set_pinmode(uint8_t pin, uint8_t flags);
  virtual void rotate_file();
  virtual void open_line(uint16_t id, uint16_t timestamp);
  virtual void close_line();
  virtual void get_file_name(char * buffer, uint8_t max_size);
  virtual char * get_file_name_ptr();
  virtual bool re_init_sd();
  virtual bool is_sd_failed();
  virtual File * get_file_ptr();

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
  File file;  // todo: make not public
  Sd_i * sd;
  SoftwareSerial * serialPort;
  ParsingState ParserState;  // stateful side-effect for parsing
};

#endif  // SRC_LOGFILE_H_
