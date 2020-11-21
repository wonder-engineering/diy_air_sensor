// Copyright 2020 Brett M. Leedy

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include "SensorArray.hh"
#include "LogFile.h"
#include "smoke_sensor.h"
#include "MQSensor.hh"
#include "AirSensorDisplay.hh"


/*------------------------------------
 * Gas sensor test harness
 *
 * MQ-2 - Smoke -
 ** MQ-3 - Alcohol Gas - https://www.sparkfun.com/products/8880
 * MQ-4 - Mehane (CNG) - https://www.sparkfun.com/products/9404
 ** MQ-5 - LPG, natural gas, town gas - https://tinyurl.com/y464ghh8
 * MQ-6 - LPG, iso-butane, propane - https://tinyurl.com/y4pnkfsl
 ** MQ-7 - Carbon Monoxide (CO) - https://tinyurl.com/zr2uqtn
 * MQ-8 - Hydrogen - https://www.sparkfun.com/products/10916
 ** MQ-9 - Gas leaks - https://wiki.seeedstudio.com/Grove-Gas_Sensor-MQ9/
 ** MQ-131 - Ozone - https://www.sparkfun.com/products/17051
 * MQ-135 - Harmful Gasses https://components101.com/sensors/mq135-gas-sensor-for-air-quality
 ** ! MQ-136 - Hydrogen Sulfide - https://www.sparkfun.com/products/17052 (spare one in yellow box)
 ** ! MQ-137 - Ammonia - https://www.sparkfun.com/products/17053
 * ! Multi-channel gas sensor: https://wiki.seeedstudio.com/Grove-Multichannel_Gas_Sensor/
 */
/*
 * Current todo list:
 *   - Collect baselines for values
 *   - Calibrate particle sensor
 */

// todo: trivialize all business logic in here and add tests to it

// Constants
#define SECONDS_PER_DAY 86400
#define SENSOR_ACCUM_RATE  0.1
#define DEFAULT_ZERO_ADJUST 0
#define DEFAULT_GAIN        1.0

// Classes for main components
SensorState       sensor_state;  // one "database" for stateful data
LogFile           * logfile;
SensorArray       * sensors;  // container for all the sensors
SmokeSensor       * dust;
AirSensorDisplay  * sensor_display;


// todo: makethe sensors container pass an iterable sensor data object out for the display and the 
// todo: create a SensorDisplay class, that you pass sensor data (and other statethings) to
// todo: create a SensorLog class, that you pass sensor data (and other state things to)


void setup() {

  // Init Serial port
  Serial.begin(115200);
  while (!Serial); // Wait for it to be initialized.

  Serial.println(F("Init Pins..."));
  pinMode(MENU_SELECT_BUTTON, INPUT_PULLUP);
  pinMode(MENU_UP_BUTTON,     INPUT_PULLUP);
  pinMode(MENU_DN_BUTTON,     INPUT_PULLUP);

  Serial.println(F("Init gas Sensors..."));
  sensors = new SensorArray();
  if(!sensor_state.device.is_alternate_config){
    sensors->add_sensor(new MQSensor(" LPG", SENSOR_ACCUM_RATE, A1, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ5 - LPG, City Gas Leak
    sensors->add_sensor(new MQSensor("  CO", SENSOR_ACCUM_RATE, A6, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ7 - Carbon Monoxide
    sensors->add_sensor(new MQSensor("Ozon", SENSOR_ACCUM_RATE, A7, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ131 - Ozone
    sensors->add_sensor(new MQSensor(" Gas", SENSOR_ACCUM_RATE, A3, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MP9 Gas leaks
    sensors->add_sensor(new MQSensor(" Haz", SENSOR_ACCUM_RATE, A2, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ135Poison Gasses (organic)
    Serial.println(F("Init Particle Sensor..."));
    // todo: roll into sensor class dust = new SmokeSensor(A0, 4, lcd, LCD_COLUMN_1, 0);
  } else {
//    sensors->add_sensor("MQ2 ", LCD_COLUMN_1, 1, A0, 0.1);  // MQ2 - smoke
//    sensors->add_sensor("MQ7 ", LCD_COLUMN_1, 2, A1, 0.1);  // MQ7 - Carbon Monoxide
//    sensors->add_sensor("MQ4 ", LCD_COLUMN_2, 0, A2, 0.1);  // MQ4 - Methane (CNG)
//    sensors->add_sensor("MQ6 ", LCD_COLUMN_2, 1, A3, 0.1);  // MQ-6 - LPG, iso-butane, propane
//    sensors->add_sensor("MQ8 ", LCD_COLUMN_2, 2, A6, 0.1);  // MQ-8 - Hydrogen
//    sensors->add_sensor("M137", LCD_COLUMN_1, 0, A7, 0.1);  // MQ-137 - Ammonia
    Serial.println(F("No Particle Sensor in config!"));
    dust = NULL;
  }

  Serial.println(F("Init LCD..."));
  sensor_display = new AirSensorDisplay();

  Serial.println(F("Init Logfile..."));
  logfile = new LogFile();

  Serial.println(F("Init done."));
}

uint32_t loop_number = 0;
uint32_t loop_start_millis = 0;

void loop() {
  loop_start_millis = millis();

  // Serial log start
  Serial.print(loop_number++);
  Serial.print(F(" ----------- "));
  Serial.println(loop_start_millis);



  // Collect and print sensor data to screen
  sensors->sense_all();
  sensors->log_all_serial_only();
  if(dust != NULL){
    dust->sense();
    dust->log_serial();
  }



  // Wrap the file every day
  if(loop_number % (SECONDS_PER_DAY / (sensor_state.device.sampling_period_ms / 1000)) == 0){
    Serial.println(F("Wrapping the log File"));
  }

  // Log to file every N loops, if file logging is configured on
  if(sensor_state.device.logging_enabled){
    if(loop_number % sensor_state.device.log_every_n_loops == 0) {
      logfile->open_line(loop_number, loop_start_millis);
      // fixme sensors->log_all(logfile->get_file_ptr());
      if(dust != NULL){dust->log(logfile->get_file_ptr());}
      logfile->close_line();
    }
  }

  // Set sensor zeros, based on menu adjustments

  // Burn remainder of the loop period
  while(millis() < loop_start_millis + sensor_state.device.sampling_period_ms) {
    // Check that millis() hasn't wrapped
    if(loop_start_millis > millis()){
      //millis have wrapped - Should happen every 50 days, give or take
      loop_start_millis = millis(); //hacky
      break;
    }

  }// while(millis)

}
