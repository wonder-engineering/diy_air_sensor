// Copyright 2020 Brett M. Leedy

#ifndef GAS_SENSOR_CUSTOM_LCD_GLYPHS_H_
#define GAS_SENSOR_CUSTOM_LCD_GLYPHS_H_

#ifdef IN_TESTING
#include "test/Stub_Arduino.h"
#endif

byte file_ok_glyph[8] = {
    0b11110,
    0b10001,
    0b11111,
    0b10001,
    0b11111,
    0b10001,
    0b11111,
  };

byte file_bad_glyph[8] = {
    0b11110,
    0b10001,
    0b11011,
    0b10101,
    0b11011,
    0b10001,
    0b11111,
  };

byte skull_glyph[8] = {
    0b00000,
    0b01110,
    0b10101,
    0b11011,
    0b01110,
    0b01110,
    0b00000,
    0b00000
};


#endif  // GAS_SENSOR_CUSTOM_LCD_GLYPHS_H_
