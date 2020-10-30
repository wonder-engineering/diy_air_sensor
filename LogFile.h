// Copyright 2020 Brett M. Leedy

#ifndef GAS_SENSOR_LOGFILE_H_
#define GAS_SENSOR_LOGFILE_H_

#ifndef IN_TESTING  // we will mock libs for tesing
  #include <Arduino.h>
  #include <SPI.h>
  #include <SD.h>
  #include <EEPROM.h>
#else
  #include "gmock/gmock.h"
  #include "gtest/gtest.h"
  #include "test/Stub_Arduino.h"
  #include "test/Stub_Serial.h"
  #include "test/File_mock.h"
#endif

#define MAX_FILENAME_LEN 12  // filenames can be a
                             // maximum of 8 characters for FAT
const char log_file_name_base[] = "LOG";
#define LOGFILE_EXTENSION "CSV"
#define SD_COOLDOWN_LENGTH 5000  // milliseconds

// SD interface to abstract external library for testing
class Sd_i{
				public:
				Sd_i();
				virtual bool begin(uint8_t csPin);
				virtual File open(const char *filename, uint8_t mode);
				virtual void end();
};


class LogFile{
  char current_name[MAX_FILENAME_LEN];
  uint16_t current_id = 0;
  bool sd_failure = true;
  uint16_t file_failure_count = 0;
  uint16_t sd_failure_count = 0;
  uint16_t cooldown_start_millis = 0;
	bool first_run=true;
  File file;  // todo: make not public
	Sd_i * sd;

 public:
  LogFile();
  virtual void rotate_file();
  virtual void open_line(uint16_t id, uint16_t timestamp);
  virtual void close_line();
  virtual void get_file_name(char * buffer, uint8_t max_size);
  virtual char * get_file_name_ptr();
  virtual bool re_init_sd();
  virtual bool is_sd_failed();
  virtual File * get_file_ptr();

	void replace_sd_interface(Sd_i * sd);
	bool get_sd_failure(){return this->sd_failure;};
	virtual bool is_first_run(){bool rv = this->first_run; this->first_run = false; return rv;};
	virtual uint16_t get_millis(){return millis();};
	void reset_first_run(){this->first_run=true;}; // for testing
 private:
  virtual uint16_t get_highest_used_id();
  virtual void override_file_number(uint16_t new_id);
  //virtual void handle_failures();
	virtual bool begin_sd();
	virtual void set_pinmode(uint8_t pin, uint8_t flags);
};

#endif  // GAS_SENSOR_LOGFILE_H_
