// Copyright 2020 Brett M. Leedy

#ifndef GAS_SENSOR_SMOKE_SENSOR_H_
#define GAS_SENSOR_SMOKE_SENSOR_H_

#ifdef IN_TESTING
#define PROGMEM
#include "test/Stub_Arduino.h"
#include "test/Stub_Serial.h"
#else
#endif

#include "Arduino.h"
#include "Sensor.hh"

/*-------------------------------------------------------------
  smoke_sensor

  This code drives a Sharp GP2Y1010AU0F sensor with control of
  a fan and LCD display.

  For calibration, achart of pm10 vs pm2.5:https://tinyurl.com/y3fkjgo7
  Best note for sample code: https://tinyurl.com/y6xhshw4
  US AQI standard: https://tinyurl.com/yxa48kty
 -------------------------------------------------------------*/
// LCD Display content constants and variables
#define DESC_LEN 30          // max length for AQI Description
#define ACCUM_FRACTION 0.05  // lower = more smooth filtering
// Dust Sensor Calibration Constants
#define DUST_DELAY_TIME_1  280
#define DUST_DELAY_TIME_2  40


// todo: add unit tests after ripping out entanglement with the logfile and lcd

class GP2YSensor : public Sensor {
 private:
   uint8_t led_power;

 public:
   GP2YSensor(const char * short_name, float accum_rate,
               uint8_t analog_pin, uint16_t zero_adjust, float gain, uint8_t led_power_pin);
  uint16_t read_sensor();
  
};

#endif  // GAS_SENSOR_SMOKE_SENSOR_H_
