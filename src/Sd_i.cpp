// Copyright 2020 Brett M. Leedy
#include "Sd_i.hh"

#ifdef IN_TESTING
#include "../test/Stub_SD.h"
#else
#include "SD.h"
#endif

Sd_i::Sd_i() {}

bool Sd_i::begin(uint8_t csPin) {
    bool rv = true;
#ifndef IN_TESTING
    rv = SD.begin(csPin);
#endif
     return rv;
}

File Sd_i::open(const char *filename, uint8_t mode) {
#ifndef IN_TESTING
    File rv = SD.open(filename, mode);
#else
    File rv = File();
#endif
    return rv;
}

void Sd_i::end() {
#ifndef IN_TESTING
    SD.end();
#endif
}  // todo: call when there's an error (card removed)
