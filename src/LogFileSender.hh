// Copyright 2020 Brett M. Leedy

/*
  LogFile class

  This class manages files written to the SD card.  It will:
    * automatically find the next-highest log number
    * write data that is passed to it
    * rotate files if they grow too large
    * try to re-init if a failure happens

  This class is the only thing aware of the log file format.  It
    reads SensorState and writes log lines to the SD card.

  NOTE: this class is not currently functioning.  It is dead code
    that needs to be re-factored and made to do the above ^^^.
*/

#ifndef SRC_LOGFILESENDER_HH_
#define SRC_LOGFILESENDER_HH_


#ifndef IN_TESTING  // we will mock libs for tesing
  #include <Arduino.h>
  #include <SPI.h>
  #include <SoftwareSerial.h>
#else
  #include <stdlib.h>
  #include "../test/Stub_Arduino.h"
  #include "../test/Stub_Serial.h"
  #include "../test/File_mock.h"
  // Stub serial interfaces are equivalent API's
  typedef HardwareSerial SoftwareSerial;
#endif

#include "../include/LogSerialInterface.hh"


class LogFileSender {
 public:
  LogFileSender();
  void rotate_file();
  void open_line(uint32_t id, uint32_t timestamp);
  void write_field(uint8_t field) {write_field((uint32_t)field);}
  void write_field(uint16_t field) {write_field((uint32_t)field);}
  void write_field(uint32_t field);
  void write_field(float field);
  void write_field(const char * field);
  void close_line();

 private:
  virtual void override_file_number(uint8_t new_id);
  virtual uint32_t get_millis() {return millis();}
  void set_pinmode(uint8_t pin, uint8_t flags);

  uint8_t current_id = 0;
  SoftwareSerial * serialPort;
};

#endif  // SRC_LOGFILESENDER_HH_
