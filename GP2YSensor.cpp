#include "GP2YSensor.hh"

GP2YSensor::GP2YSensor(const char * short_name, float accum_rate,
               uint8_t analog_pin, uint16_t zero_adjust, float gain, uint8_t led_power_pin) :
               Sensor{short_name, accum_rate, analog_pin, zero_adjust, gain}  {

  this->led_power = led_power_pin;
  pinMode(led_power_pin, OUTPUT);  // Turns sensor LED on/off

  Serial.println(F("Smoke sensor initialized on pins:"));
  Serial.print(F("pwr: ")); Serial.println(analog_pin);
  Serial.print(F("led: ")); Serial.println(led_power_pin);
}


uint16_t GP2YSensor::read_sensor() {  //////////////////////////////////////////////////////////
  // Note: this will block all other actions while I'm sensing
  digitalWrite(led_power, LOW);              // power on sensor LED
  delayMicroseconds(DUST_DELAY_TIME_1);           // wait for data collection
  this->last_raw = analogRead(this->pin);    // read sensor value
  delayMicroseconds(DUST_DELAY_TIME_2);           // wait for a settling time
  digitalWrite(led_power, HIGH);             // power off the sensor LED

  return this->last_raw;
}
