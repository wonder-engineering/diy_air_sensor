// Copyright 2020 Brett M Leedy
/**
 * Arduino Serial mock
 */
#ifndef TEST_STUB_SD_H_
#define TEST_STUB_SD_H_

#include <stdint.h>
#include "Stub_Arduino.h"
#include "File_mock.h"


class SD_ {
 public:
    SD_() {}
    virtual bool begin(uint8_t csPin) {return true;}
    virtual File open(const char *filename, uint8_t mode) {return File();}
    virtual void end() {}
} SD;
#endif  // TEST_STUB_SD_H_
