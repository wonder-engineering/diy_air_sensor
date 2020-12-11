// Copyright 2020 Brett M. Leedy
/*
 * Arduino Serial mock
 */
#ifndef TEST_STUB_SERIAL_H_
#define TEST_STUB_SERIAL_H_

#include <stdint.h>
#include "Stub_Arduino.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

typedef unsigned long size_t;  // NOLINT

class HardwareSerial {
 private:
  static bool printToCout;

 public:
  static void setPrintToCout(bool flag) {}
  HardwareSerial() {}
  HardwareSerial(uint8_t x, uint8_t y) {}

  static size_t print(const char[]) {return 0;}
  static size_t print(char) {return 0;}
  static size_t print(unsigned char, int = DEC) {return 0;}
  static size_t print(int, int = DEC) {return 0;} // NOLINT
  static size_t print(unsigned int, int = DEC) {return 0;} // NOLINT
  static size_t print(long, int = DEC) {return 0;} // NOLINT
  static size_t print(unsigned long, int = DEC) {return 0;}  //NOLINT
  static size_t print(double, int = 2) {return 0;}

  static size_t println(const char[]) {return 0;}
  static size_t println(char) {return 0;}
  static size_t println(unsigned char, int = DEC) {return 0;}
  static size_t println(int, int = DEC) {return 0;}
  static size_t println(unsigned int, int = DEC) {return 0;}
  static size_t println(long, int = DEC) {return 0;} // NOLINT
  static size_t println(unsigned long, int = DEC) {return 0;} // NOLINT
  static size_t println(double, int = 2) {return 0;}
  static size_t println(void) {return 0;}

  size_t write(uint8_t) {return 0;}
  size_t write(const char *str) {return 0;}
  // size_t write(const uint8_t *buffer, size_t size) {return 0;}

  virtual uint8_t begin(uint32_t) {return 0;}

  uint8_t available() {return 0;}
  uint8_t read() {return 0;}

  static void flush() {}
};
extern "C" HardwareSerial Serial;

#endif  // TEST_STUB_SERIAL_H_
