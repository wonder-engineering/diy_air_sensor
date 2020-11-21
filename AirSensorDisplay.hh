#ifndef AIR_SENSOR_DISPLAY_HH
#define AIR_SENSOR_DISPLAY_HH

#include "SensorState.hh"
#include "SensorMenu.h"

// Indices of LCD columns
#define LCD_NUM_ROWS     4
#define LCD_NUM_COLUMNS 20
#define LCD_SENSOR_COLUMNS 10 // number of columns reserved for a sensor to display

// LCD Glyph ID's for this sketch
#define FILE_OK_GLYPH  0
#define FILE_BAD_GLYPH 1
#define SKULL_GLYPH    2


/*
 * This class contains the project-specific design of the display.
 *  It ingests data from the sensors and (optionally) has a menu to change
 *  the configuration of the air sensor unit.
 * 
 */
 
class AirSensorDisplay {
 public:
  void add_menu(SensorMenu * menu);
  void display_data(SensorState * sensor_state);
  AirSensorDisplay();
  AirSensorDisplay(SensorMenu * menu);

 private:
  LiquidCrystal_I2C * lcd;
  SensorMenu * menu;
  void init_lcd();
};

#endif
