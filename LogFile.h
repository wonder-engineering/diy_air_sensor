// Copyright 2020 Brett M. Leedy

#ifndef GAS_SENSOR_LOGFILE_H_
#define GAS_SENSOR_LOGFILE_H_

#ifndef IN_TESTING  // we will mock libs for tesing
  #include <Arduino.h>
  #include <SPI.h>
  #include <SD.h>
  #include <EEPROM.h>
#endif

#define MAX_FILENAME_LEN 12  // filenames can be a
                             // maximum of 8 characters for FAT
const char log_file_name_base[] = "LOG";
#define LOGFILE_EXTENSION "CSV"
#define SD_COOLDOWN_LENGTH 5000  // milliseconds

class LogFile{
  char current_name[MAX_FILENAME_LEN];
  uint16_t current_id = 0;
  bool sd_failure = true;
  uint16_t file_failure_count = 0;
  uint16_t sd_failure_count = 0;
  uint16_t cooldown_start_millis = 0;
  File file;  // todo: make not public

 public:
  LogFile();
  void rotate_file();
  void open_line(uint16_t id, uint16_t timestamp);
  void close_line();
  void get_file_name(char * buffer, uint8_t max_size);
  char * get_file_name_ptr();
  virtual bool re_init_sd();
  bool is_sd_failed();
  File * get_file_ptr();

 private:
  uint16_t get_highest_used_id();
  void override_file_number(uint16_t new_id);
  void handle_failures();
};

#endif  // GAS_SENSOR_LOGFILE_H_
