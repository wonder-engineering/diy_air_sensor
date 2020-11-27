// Copyright 2020 Brett M Leedy
/**
 * Stub Arduino
 */
#ifndef TEST_STUB_ARDUINO_H_
#define TEST_STUB_ARDUINO_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#ifdef WIN32
#elif linux
#else
#define true 0x1
#define false 0x0
#endif

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

#define SERIAL  0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define CHANGE 1
#define FALLING 2
#define RISING 3

#define A0 0
#define A1 1
#define A2 2
#define A3 3
#define A4 4
#define A5 5
#define A6 6
#define A7 7

typedef uint8_t boolean;
typedef uint8_t byte;

void init(void);

void pinMode(uint8_t, uint8_t);
void digitalWrite(uint8_t, uint8_t);
int digitalRead(uint8_t);
int analogRead(uint8_t);
void analogReference(uint8_t mode);
void analogWrite(uint8_t, int);  // NOLINT

unsigned long millis(void);  // NOLINT

unsigned long micros(void);  // NOLINT
void delay(unsigned long);  // NOLINT
void delayMicroseconds(unsigned int us);  // NOLINT
unsigned long pulseIn(uint8_t pin, uint8_t state,  // NOLINT
                        unsigned long timeout);   // NOLINT

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

void attachInterrupt(uint8_t, void (*)(void), int mode);  // NOLINT
void detachInterrupt(uint8_t);  // NOLINT

void setup(void);
void loop(void);

uint16_t pgm_read_word(const char *const *);
#define strcpy_P strcpy

#define F(x) (x)
#define UNUSED(expr) do { (void)(expr); } while (0)

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // TEST_STUB_ARDUINO_H_
