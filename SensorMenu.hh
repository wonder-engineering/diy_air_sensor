// Copyright 2020 Brett M. Leedy
/*
  SensorMenu class

  This class defines everything about how the menu for the sensor device
    behaves.  It does work on the SensorState which is passed into it by
    reference.
  Change this class if you want to:
    * Add a new config to be changed by the menu
    * Re-order the menu
    * Change the look and feel of the menu
    * Change the button interactions
*/


#ifndef SENSORMENU_HH_
#define SENSORMENU_HH_


#ifdef IN_TESTING
// just skip progmem macro for test builds
#define PROGMEM
#include "test/Stub_Arduino.h"
#include "test/Stub_Serial.h"
#include "test/Stub_LiquidCrystal_I2C.hh"
#else
#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#endif

#include "SensorState.hh"


// Menu Buttons
#define MENU_SELECT_BUTTON 8
#define MENU_UP_BUTTON     6
#define MENU_DN_BUTTON     5
#define MENU_LENGTH        14

#define EXIT_FROM_MENU true
#define REMAIN_IN_MENU false

// Menu is stored in progmem to save RAM, which means it has to
//  be more static here than I'd like
const char menu_e[]      PROGMEM = "EXIT";                   // 0
const char menu_raw[]    PROGMEM = "Disp. Raw / Average";    // 1
const char menu_file[]   PROGMEM = "File";                   // 2
const char menu_sam[]    PROGMEM = "Sampling Rate";          // 3
const char menu_log[]    PROGMEM = "File logging Rate";      // 4
const char menu_back[]   PROGMEM = "Toggle LCD Backlight";   // 5
const char menu_logon[]  PROGMEM = "Toggle file log";        // 6
const char menu_lpg[]    PROGMEM = "LPG Threshold";          // 7
const char menu_co[]     PROGMEM = "CO Threshold";           // 8
const char menu_ozone[]  PROGMEM = "Ozone Threshold";        // 9
const char menu_gas[]    PROGMEM = "Gas Threshold";          // 10
const char menu_poison[] PROGMEM = "Poison Threshold";       // 11
const char menu_dust_z[] PROGMEM = "Dust Zero";              // 12
const char menu_sens_t[] PROGMEM = "Sensor Type";            // 13

// vector-ize these, if I can
const char *const menu_line[] PROGMEM = {
           menu_e,       //  0: Exit
           menu_raw,     //  1: Raw or averaged data
           menu_file,    //  2: Increment the file name
           menu_sam,     //  3: Set sampling frequency
           menu_log,     //  4: Set logging rate (every n samples)};
           menu_back,    //  5: Toggle LCD backlight
           menu_logon,   //  6: Toggle logging to file
           menu_lpg,     //  7: LPG Gas Sensor
           menu_co,      //  8: Carbon Monoxide Sensor
           menu_ozone,   //  9: Ozone Sensor
           menu_gas,     // 10: Gas Sensor
           menu_poison,  // 11: Hazardous Gas Sensor
           menu_dust_z,  // 12: Dust sensor adjustment
           menu_sens_t   // 13: Sensor configuration type
         };




class SensorMenu {
 public:
  explicit SensorMenu(LiquidCrystal_I2C * lcd);
  void enter_menu(SensorState * state);

 protected:
  void render_menu(uint8_t line);
  bool enter_menu_item(uint8_t id, SensorState * state);
  void init(LiquidCrystal_I2C * lcd);
  bool display_sensor_setting(const char * name, uint16_t * setting);
  virtual bool sensor_settings_callback(const char * name,
                                        uint16_t * setting,
                                        SensorSettings * settings);
  virtual bool exit_callback();
  virtual bool backlight_callback(SensorSettings * settings);
  virtual bool logon_callback(SensorSettings * settings);
  virtual bool sensor_t_callback(SensorSettings * settings);
  virtual void restart();
  void display_lograte_menu(SensorSettings * settings);
  virtual bool lograte_callback(SensorSettings * settings);
  void display_sampling_menu(SensorSettings * settings);
  virtual bool sampling_callback(SensorSettings * settings);
  void display_file_menu(SensorState * state);
  virtual bool file_callback(SensorState * state);
  virtual bool disp_callback(SensorSettings * settings);
  virtual bool error_callback();
  LiquidCrystal_I2C * lcd;
  void wait_for_button_up();
};
#endif  // SENSORMENU_HH_
