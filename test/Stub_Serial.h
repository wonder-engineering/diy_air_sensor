/**
 * Arduino Serial mock
 */
#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include "Stub_Arduino.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

typedef unsigned long size_t;

class Serial_ {

  private:
    static bool printToCout;

  public:
    static void setPrintToCout(bool flag){};

  public:
    static std::size_t print(const char[]){return 0;};
    static size_t print(char){return 0;};
    static size_t print(unsigned char, int = DEC){return 0;};
    static size_t print(int, int = DEC){return 0;};
    static size_t print(unsigned int, int = DEC){return 0;};
    static size_t print(long, int = DEC){return 0;};
    static size_t print(unsigned long, int = DEC){return 0;};
    static size_t print(double, int = 2){return 0;};

    static size_t println(const char[]){return 0;};
    static size_t println(char){return 0;};
    static size_t println(unsigned char, int = DEC){return 0;};
    static size_t println(int, int = DEC){return 0;};
    static size_t println(unsigned int, int = DEC){return 0;};
    static size_t println(long, int = DEC){return 0;};
    static size_t println(unsigned long, int = DEC){return 0;};
    static size_t println(double, int = 2){return 0;};
    static size_t println(void){return 0;};

    size_t write(uint8_t){return 0;};
    size_t write(const char *str){return 0;};
    size_t write(const uint8_t *buffer, size_t size){return 0;};

    uint8_t begin(uint32_t){return 0;};

    uint8_t available(){return 0;};
    uint8_t read(){return 0;};

    static void flush(){};
};
extern Serial_ Serial;

// SerialMock* serialMockInstance(){return NULL;};
// void releaseSerialMock(){};


#endif // SERIAL_H
