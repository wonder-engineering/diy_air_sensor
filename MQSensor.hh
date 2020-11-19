// Copyright 2020 Brett M. Leedy

#ifndef DIY_AIR_SENSOR_MQ_SENSOR_H_
#define DIY_AIR_SENSOR_MQ_SENSOR_H_
// todo: use c++ .hh header conventions

#include "Sensor.hh"


class MQSensor : public Sensor {
 public:
  MQSensor(char * short_name, float accum_rate,
               uint8_t analog_pin, uint16_t zero_adjust, float gain) :
               Sensor{short_name, accum_rate, analog_pin, zero_adjust, gain} {}

 private:
  uint16_t read_sensor();
};

#endif