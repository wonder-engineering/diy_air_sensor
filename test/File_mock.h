// Copyright 2020 Brett M Leedy
#ifndef TEST_FILE_MOCK_H_
#define TEST_FILE_MOCK_H_

#include "Stub_Arduino.h"

#define FILE_WRITE 0
#define FILE_READ 1

// Fake class to mock and make assertions about
// this is necessary because we're not actually able
// to build the SD library for this platform,
// or rather won't get value out of spending the time.
//
class File {
 public:
  File() {}
  char * name() {return NULL;}
  File openNextFile() {return File();}
  int open() {return 0;}
  int close() {return 0;}
  virtual void print(int t) {}
  virtual void print(const char[]) {}
  void println(const char[]) {}
  virtual bool isDirectory() {return false;}
  virtual void write(uint8_t writeByte) {}
  virtual operator bool() {return false;}
};


#endif  // TEST_FILE_MOCK_H_
