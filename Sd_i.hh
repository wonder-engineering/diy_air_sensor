// Copyright 2020 Brett M. Leedy
/*
  Sd_i class

  Wrapper for SD interface.
*/
#ifndef SD_I_HH
#define SD_I_HH

#ifdef IN_TESTING
#include "test/File_mock.h"
#include "test/Stub_Arduino.h"
#else
#include "SD.h"
#endif

class Sd_i{
    public:
    Sd_i();
    virtual bool begin(uint8_t csPin);
    virtual File open(const char *filename, uint8_t mode);
    virtual void end();
};
#endif
