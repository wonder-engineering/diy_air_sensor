/**
 * Arduino mock
 *    functions
 */
#include "Stub_Arduino.h"
void pinMode(uint8_t, uint8_t){};
void digitalWrite(uint8_t, uint8_t){};
int digitalRead(uint8_t){return 0;};
int analogRead(uint8_t){return 0;};
void analogReference(uint8_t mode){};
void analogWrite(uint8_t, int){};

unsigned long millis(void){return 0;};
unsigned long micros(void){return 0;};
void delay(unsigned long){};
void delayMicroseconds(unsigned int us){};
unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout){return 0;};

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val){};
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder){return 0;};

void attachInterrupt(uint8_t, void (*)(void), int mode){};
void detachInterrupt(uint8_t){};
uint16_t pgm_read_word(const char *const *){return 0;}
