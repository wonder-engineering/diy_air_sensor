// Copyright 2020 Brett M. Leedy
/*
 * AirSensorDisplay class.
 *
 * This class is responsible for the overall structure of the LCD
 *  display.  It initializes the LCD and then will refresh the display
 *  every time you call display_data().  at the time of the display_data()
 *  call, if any buttons are pressed it will enter menu mode and do work
 *  on the SensorState data structure, depennding on the menu interaction.
 *
 * Modify this class if you want to change the appearance of the sensor
 *  display.
*/

#ifndef AIRSENSORDISPLAY_HH_
#define AIRSENSORDISPLAY_HH_

#include "SensorState.hh"
#include "SensorMenu.hh"
#ifdef IN_TESTING
#include "test/Stub_LiquidCrystal_I2C.hh"
#else
#include "LiquidCrystal_I2C.h"
#endif

// Indices of LCD columns
#define LCD_NUM_ROWS     4
#define LCD_NUM_COLUMNS 20
#define LCD_SENSOR_COLUMNS 10  // columns reserved for a sensor to display

// LCD Glyph ID's for this class
#define FILE_OK_GLYPH  0
#define FILE_BAD_GLYPH 1
#define SKULL_GLYPH    2

#define DISPLAY_DATA_PRECISION 1

/*
 * This class contains the project-specific design of the display.
 *  It ingests data from the sensors and (optionally) has a menu to change
 *  the configuration of the air sensor unit.
 *
 */

class AirSensorDisplay {
 public:
  // Refreshes the LCD display based on state passed in
  // Also check whether to enter the menu
  virtual void display_data(SensorState * sensor_state);

  // Initializes the LCD and menu
  AirSensorDisplay();

 private:
  LiquidCrystal_I2C * lcd;
  SensorMenu * menu;
  void init_lcd();
};

#endif  // AIRSENSORDISPLAY_HH_
