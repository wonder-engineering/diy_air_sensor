/**
 * Arduino Serial mock
 */
#ifndef SD_H
#define SD_H

#include <stdint.h>
#include "Stub_Arduino.h"
#include "File_mock.h"


class SD_{
    public:
    SD_(){};
    virtual bool begin(uint8_t csPin){return true;};
    virtual File open(const char *filename, uint8_t mode){return File();};
    virtual void end(){};
} SD;


// SerialMock* serialMockInstance(){return NULL;};
// void releaseSerialMock(){};


#endif // SD
