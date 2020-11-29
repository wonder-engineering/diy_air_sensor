// Copyright 2020 Brett M. Leedy

#ifndef SRC_GP2YSENSOR_HH_
#define SRC_GP2YSENSOR_HH_

#ifdef IN_TESTING
#define PROGMEM
#include "../test/Stub_Arduino.h"
#include "../test/Stub_Serial.h"
#else
#endif

#include "Sensor.hh"

/*
  GP2YSensor

  This code drives a Sharp GP2Y1010AU0F sensor with control of
  a fan and LCD display.

  See Sensor.hh for all public methods.

  For calibration, achart of pm10 vs pm2.5:https://tinyurl.com/y3fkjgo7
  Best note for sample code: https://tinyurl.com/y6xhshw4
  US AQI standard: https://tinyurl.com/yxa48kty
*/

#define DUST_DELAY_TIME_1  280
#define DUST_DELAY_TIME_2  40

class GP2YSensor : public Sensor {
 public:
  GP2YSensor(const char * short_name, float accum_rate,
               uint8_t analog_pin, uint16_t zero_adjust,
               float gain, uint8_t led_power_pin);

 private:
  uint8_t led_power;
  uint16_t read_sensor();  // all sensor read implementation goes here
};

#endif  // SRC_GP2YSENSOR_HH_
