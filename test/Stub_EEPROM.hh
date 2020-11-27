// Copyright 2020 Brett M Leedy

#ifndef TEST_STUB_EEPROM_HH_
#define TEST_STUB_EEPROM_HH_

#include <inttypes.h>

typedef void * EEPtr;

class EEPROMClass {
 public:
  // Basic user access methods.
  uint8_t read(int idx ) {return 0;}
  void write(int idx, uint8_t val ) {}

  // STL and C++11 iteration capability.
  EEPtr begin()                        { return 0x00; }

  //  Functionality to 'get' and 'put' objects to and from EEPROM.
  template< typename T > T &get(int idx, T &t ) {  // NOLINT
      return t;
  }

  template< typename T > const T &put(int idx, const T &t ) {
      return t;
  }

  template< typename T > const T &update(int idx, const T &t ) {
      return t;
  }
}EEPROM;

#endif  // TEST_STUB_EEPROM_HH_
