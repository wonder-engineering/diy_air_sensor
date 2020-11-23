// Copyright 2020 Brett M. Leedy
/*
  Sd_i class

  Wrapper for SD interface.
*/
#ifndef SD_I_HH
#define SD_I_HH

#include "SD.h"

class Sd_i{
    public:
    Sd_i();
    virtual bool begin(uint8_t csPin);
    virtual File open(const char *filename, uint8_t mode);
    virtual void end();
};
#endif
