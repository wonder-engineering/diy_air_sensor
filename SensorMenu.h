// Copyright 2020 Brett M. Leedy
// todo: move code to cpp for reusabiltiy.
// todo: This classcurrently takes in pointers to  other things it has no business
//   changing and calls methods to things.  In an ideal world, this class will only
//   be responsible for updating a config, that other things will then consume as
//   the user-input state.  This is a very high priority.


// todo:
// Factor out smoke sensor
// Factor out logfile

#ifndef GAS_SENSOR_SENSORMENU_H_
#define GAS_SENSOR_SENSORMENU_H_

#define EXIT_FROM_MENU true
#define REMAIN_IN_MENU false

#ifdef IN_TESTING
// just skip progmem macro for test builds
#define PROGMEM
#include "test/Stub_Arduino.h"
#include "test/Stub_Serial.h"
#include "test/Stub_LiquidCrystal_I2C.hh"
#include "test/Stub_EEPROM.hh"
#include "smoke_sensor.h"  // todo: include real files

#else
// non-test includes
#include <EEPROM.h>

#endif

#include "AnalogSensor.h" // todo: include real files

#include "LogFile.h"
#include <stdint.h>

// Menu Buttons
#define MENU_SELECT_BUTTON 8
#define MENU_UP_BUTTON     6
#define MENU_DN_BUTTON     5

#define MAX_SENSORS        8  // todo: make sure I check that this is not exceeded

#define MENU_LENGTH 14

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

#define DEFAULT_LOOP_PERIOD_MILLIS 2000  // every one second
#define DEFAULT_LOG_EVERY_N_LOOPS    10  // every ten seconds

struct settings_type {
  uint16_t sampling_period_ms = DEFAULT_LOOP_PERIOD_MILLIS;
  uint16_t log_every_n_loops = DEFAULT_LOG_EVERY_N_LOOPS;
  uint16_t file_number = 0;

  uint16_t dust_zero = 0;
  uint16_t sensor_thresholds[MAX_SENSORS];
  uint16_t sensor_zeros[MAX_SENSORS];

  bool log_raw = false;
  bool backlight = true;
  bool logging = true;
  bool alt_sensor_config = false;

  uint16_t checksum = 0;

  uint16_t calc_checksum() {
    uint16_t rv = 198;
    rv += sampling_period_ms;
    rv += log_every_n_loops;
    rv += log_raw;
    rv += file_number;
    rv += dust_zero;
    rv += backlight;
    rv += alt_sensor_config;
    for (int i = 0; i < MAX_SENSORS; i++) {
	    rv += sensor_thresholds[i];
    }
    for (int i=0; i < MAX_SENSORS; i++) {
	    rv += sensor_zeros[i];
    }
    rv += logging;
    return rv;
  }
  void store_checksum() {
    checksum = calc_checksum();
  }
  bool check() {
    return (checksum == calc_checksum());
  }
  void dump() {
    Serial.println(F("Configuration:"));
    Serial.print(F("  sampling_period_ms: "));
    Serial.println(this->sampling_period_ms);
    Serial.print(F("  log_every_n_loops: "));
    Serial.println(this->log_every_n_loops);
    Serial.print(F("  log_raw (boolean): "));
    Serial.println(this->log_raw);
    Serial.print(F("  file_number: "));
    Serial.println(this->file_number);
    Serial.print(F("  backlight: "));
    Serial.println(this->backlight);
    Serial.print(F("  logging: "));
    Serial.println(this->logging);
    Serial.print(F("  checksum: "));
    Serial.println(this->checksum);
    Serial.print(F("  alt_sensor_config: "));
    Serial.println(this->alt_sensor_config);
    Serial.print(F("  calculated checksum: "));
    Serial.println(this->calc_checksum());
  }
};



class SensorMenu {
  LiquidCrystal_I2C * lcd = NULL;
  LogFile * logfile = NULL;  // todo: break this dependency
  AnalogSensor * sensors = NULL;  // todo: break this dependency
  SmokeSensor * dust = NULL;  // todo: break this dependency
  uint8_t col1, col2;
  settings_type config;

 public:
  SensorMenu(LiquidCrystal_I2C * lcd,
             uint8_t col1_idx,
             uint8_t col2_idx) {
    init(lcd, col1_idx, col2_idx);
  }

  void init(LiquidCrystal_I2C * lcd,
             uint8_t col1_idx,
             uint8_t col2_idx) {
    this->lcd = lcd;
    this->col1 = col1_idx;
    this->col2 = col2_idx;

    for (int i=0; i < MAX_SENSORS; i++) {
      config.sensor_zeros[i] = 0;  // todo: define these based on menu options
    }

    // retrieve config values from the EEPROM
    load_settings_from_eeprom(0, &this->config);
    if (this->config.check()) {
      this->config.dump();
    } else {
      Serial.println(F("Checksum failed! Writing default config."));
      this->config.sampling_period_ms = DEFAULT_LOOP_PERIOD_MILLIS;
      this->config.log_every_n_loops = DEFAULT_LOG_EVERY_N_LOOPS;
      this->config.log_raw = false;
      this->config.file_number = 0;
      this->config.backlight = true;
      this->config.logging = true;
      this->config.alt_sensor_config = false;
      commit_config();  // write to EEPROM with valid checksum
    }
  }

  // light wrapper for better testing - DO NOT ADD LOGIC HERE
  virtual void load_settings_from_eeprom(int idx, settings_type * myconfig){
    EEPROM.get(idx, *myconfig);
  }

  uint16_t get_sensor_threshold(uint8_t id) {
    return this->config.sensor_thresholds[id];
  }

  bool get_logon_config(){
    return this->config.logging;
  }

  bool get_file_number_config(){
    return this->config.file_number;
  }

  bool get_alt_sensor_config(){
    return this->config.alt_sensor_config;
  }

  void attach_logfile(LogFile * logfile) {
    this->logfile = logfile;
  }

  void attach_dust_sensor(SmokeSensor * dust) {
    this->dust = dust;
  }

  LiquidCrystal_I2C * get_lcd(){
    return this->lcd;
  }

  bool check_config(){
    return this->config.check();
  }

  void attach_analog_sensors(AnalogSensor * sensors) {
    this->sensors = sensors;
  }

  void wait_for_button_up() {
    while (digitalRead(MENU_SELECT_BUTTON) == LOW ||
           digitalRead(MENU_UP_BUTTON) == LOW ||
           digitalRead(MENU_DN_BUTTON) == LOW) {
            // do nothing until all buttons released for at least one cycle
            delay(1);
          }
  }

  bool display_sensor_setting(const char * name, uint16_t * setting) {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(name);
    lcd->setCursor(0, 1);
    lcd->print(F("Setting : "));
    lcd->print(*setting);
    lcd->setCursor(0, 3);
    lcd->print(F("Blue to exit."));

    return REMAIN_IN_MENU;
  }


  virtual bool sensor_settings_callback(const char * name, uint16_t * setting) {
    Serial.print(F("Entered settings callback for ")); Serial.println(name);

    this->display_sensor_setting(name, setting);

    wait_for_button_up();
    while (true) {
      if (digitalRead(MENU_SELECT_BUTTON) == LOW) {
        commit_config();  // write to EEPROM before exiting
        wait_for_button_up();
        return REMAIN_IN_MENU;
      } else if (digitalRead(MENU_UP_BUTTON) == LOW) {
        *setting += 20;
        this->display_sensor_setting(name, setting);
        wait_for_button_up();
      } else if (digitalRead(MENU_DN_BUTTON) == LOW) {
        if ( *setting >= 20)
          *setting -= 20;
        this->display_sensor_setting(name, setting);
        wait_for_button_up();
      }
    }
  }

  virtual bool exit_callback() {
    return EXIT_FROM_MENU;
  }

  virtual void commit_config() {
    this->config.store_checksum();
    EEPROM.put(0, this->config);
  }

  uint16_t get_log_every_n_loops() {
    return this->config.log_every_n_loops;
  }

  bool get_backlight_config() {
    return this->config.backlight;
  }

  virtual bool backlight_callback() {
    this->config.backlight = !this->config.backlight;
    commit_config();
    return EXIT_FROM_MENU;  // exit back out to main display
  }

  bool get_log_raw() {
    return this->config.log_raw;
  }

  virtual bool logon_callback() {
    this->config.logging = !this->config.logging;
    commit_config();
    return EXIT_FROM_MENU;  // exit back out to main display
  }

  virtual bool sensor_t_callback() {
    if (this->config.alt_sensor_config)
      this->config.alt_sensor_config = false;
    else
      this->config.alt_sensor_config = true;
    commit_config();
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("Restart in "));
    for (int i=3; i >= 0; i--) {
      lcd->print(i);
      lcd->print(".");
      delay(1000);
    }
    // This is heavy-handed, but we need to re-instantiate all sensors
    // plus restart logging.
    this->restart();
    return REMAIN_IN_MENU;  // never actually gets here
  }

  virtual void restart() {
    void(* resetFunc) (void) = 0;
    resetFunc();
  }

  bool is_alternate_config() {
    return this->config.alt_sensor_config;
  }

  void display_lograte_menu() {
    Serial.println(F("Entered log rate Callback"));
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("Logging every N "));
    lcd->setCursor(0, 1);
    lcd->print(F("samples:  "));
    lcd->print(this->config.log_every_n_loops);
    lcd->setCursor(0, 2);
    lcd->print(F("Use arrows to set."));
    lcd->setCursor(0, 3);
    lcd->print(F("Blue to exit."));
  }

  virtual bool lograte_callback() {
      display_lograte_menu();
      wait_for_button_up();
      while (true) {
        if (digitalRead(MENU_SELECT_BUTTON) == LOW) {
          commit_config();  // write to EEPROM before exiting
          wait_for_button_up();
          return REMAIN_IN_MENU;
        } else if (digitalRead(MENU_UP_BUTTON) == LOW) {
          this->config.log_every_n_loops++;
          this->display_lograte_menu();
          wait_for_button_up();
        } else if (digitalRead(MENU_DN_BUTTON) == LOW) {
          if ( this->config.sampling_period_ms > 1)
            this->config.log_every_n_loops--;
          this->display_lograte_menu();
          wait_for_button_up();
        }
      }
  }


  void display_sampling_menu() {
    Serial.println(F("Entered sample rate Callback"));
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("Current sample rate:"));
    lcd->setCursor(0, 1);
    lcd->print(F("Period, ms: "));
    lcd->print(this->config.sampling_period_ms);
    lcd->setCursor(0, 2);
    lcd->print(F("Use arrows to set."));
    lcd->setCursor(0, 3);
    lcd->print(F("Blue to exit."));
  }

  virtual bool sampling_callback() {
      display_sampling_menu();
      wait_for_button_up();
      while (true) {
        if (digitalRead(MENU_SELECT_BUTTON) == LOW) {
          commit_config();  // write to EEPROM before exiting
          wait_for_button_up();
          return REMAIN_IN_MENU;
        } else if (digitalRead(MENU_UP_BUTTON) == LOW) {
          this->config.sampling_period_ms += 500;
          this->display_sampling_menu();
          wait_for_button_up();
        } else if (digitalRead(MENU_DN_BUTTON) == LOW) {
          if (this->config.sampling_period_ms >= 1000)
            this->config.sampling_period_ms -= 500;
          this->display_sampling_menu();
          wait_for_button_up();
        }
      }
  }


  uint16_t get_sampling_period_ms() {
    return this->config.sampling_period_ms;
  }

  void display_file_menu() {
    Serial.println(F("Entered file Callback"));
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("Current File: "));
    lcd->setCursor(0, 1);
    lcd->print(F("   "));
    lcd->print(logfile->get_file_name_ptr());
    lcd->setCursor(0, 2);
    lcd->print(F("Use arrows to set."));
    lcd->setCursor(0, 3);
    lcd->print(F("Blue to exit."));
  }

  virtual bool file_callback() {
    if (logfile == NULL)
      return REMAIN_IN_MENU;  // just leave if there's no file attached

    display_file_menu();
    wait_for_button_up();
    while (true) {
      if (digitalRead(MENU_SELECT_BUTTON) == LOW) {
        commit_config();  // write to EEPROM before exiting
        wait_for_button_up();
        return REMAIN_IN_MENU;
      } else if (digitalRead(MENU_UP_BUTTON) == LOW ||
                  digitalRead(MENU_DN_BUTTON) == LOW) {
        logfile->rotate_file();
        display_file_menu();
        wait_for_button_up();
      }
    }
  }

  bool display_raw = false;
  virtual bool disp_callback() {
    Serial.println(F("Entered Display Callback"));
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("Press up or down to "));
    lcd->setCursor(0, 1);
    lcd->print(F("   set display type."));
    lcd->setCursor(0, 2);
    lcd->print(F("Blue button to exit."));
    lcd->setCursor(7, 3);
    if (display_raw)
      lcd->print(F("Raw    "));
    else
      lcd->print(F("Average"));

    while (true) {
      if (digitalRead(MENU_SELECT_BUTTON) == LOW) {
        if (dust    != NULL) {dust->set_display_raw(display_raw);}
        if (sensors != NULL) {sensors->set_display_raw(display_raw);}
        wait_for_button_up();
        return REMAIN_IN_MENU;
      } else if (digitalRead(MENU_UP_BUTTON) == LOW ||
                  digitalRead(MENU_DN_BUTTON) == LOW) {
        display_raw = !display_raw;
        lcd->setCursor(7, 3);
        if (display_raw)
          lcd->print(F("Raw    "));
        else
          lcd->print(F("Average"));
        wait_for_button_up();  // wait a  moment to check again
      }
    }
  }


  // Render the menu, starting with a specific line
  void render_menu(uint8_t line) {
    lcd->clear();
    for (int d_row=line; d_row < line + 4 && d_row < MENU_LENGTH; d_row++) {
      char buffer[21];  // make sure this is large enough for the largest string  todo: magic number
      strcpy_P(buffer,
        reinterpret_cast<char *>(pgm_read_word(&(menu_line[d_row]))));
      lcd->setCursor(col1, d_row-line);
      lcd->print(buffer);
      Serial.print("render line "); Serial.println(d_row);
    }
    lcd->setCursor(0, 0);
    lcd->cursor();
  }

  bool enter_menu_item(uint8_t id) {
    Serial.print(F("entering menu item ")); Serial.println(id);
    bool rv = REMAIN_IN_MENU;
    switch (id) {
      case 0:
        rv = exit_callback(); break;
      case 1:
        rv = disp_callback(); break;
      case 2:
        if (logfile != NULL) {rv = file_callback();} break;
      case 3:
        rv = sampling_callback(); break;
      case 4:
        if (logfile != NULL) {rv = lograte_callback();} break;
      case 5:
        rv = backlight_callback(); break;
      case 6:
        if (logfile != NULL) {rv = logon_callback();} break;
      case 7:
        rv = sensor_settings_callback("LPG",
              &this->config.sensor_thresholds[0]);
        break;  //  7: LPG Gas Sensor
      case 8:
        rv = sensor_settings_callback("CO",
              &this->config.sensor_thresholds[1]);
        break;  //  8: Carbon Monoxide Sensor
      case 9:
        rv = sensor_settings_callback("O3",
              &this->config.sensor_thresholds[2]);
        break;  //  9: Ozone Sensor
      case 10:
        rv = sensor_settings_callback("GAS",
              &this->config.sensor_thresholds[3]);
        break;  // 10: Gas Sensor
      case 11:
        rv = sensor_settings_callback("HAZ",
              &this->config.sensor_thresholds[4]);
        break;  // 11: Hazardous Gas Sensor
      case 12:
        rv = sensor_settings_callback("PM",  &this->config.dust_zero);
        break;  // 12: Dust sensor adjustment
      case 13:
        rv = sensor_t_callback();
        break;  // 13: toggle the set of sensors I'll use on next restart
      default:
        Serial.println(F("No function exists for this menu item"));
        error_callback();
    }
    return rv;
  }

  virtual bool error_callback(){ return false;}

  void enter_menu() {
    lcd->backlight();
    uint8_t menu_pos = 0;
    render_menu(menu_pos);  // render the menu at the start
    // now
    while (true) {
      // look for the up, down, or select buttons
      if (digitalRead(MENU_UP_BUTTON) == LOW && menu_pos > 0) {
        menu_pos--;
        render_menu(menu_pos);
        wait_for_button_up();
      } else if (digitalRead(MENU_DN_BUTTON) == LOW &&
                  menu_pos < MENU_LENGTH-1) {
        menu_pos++;
        render_menu(menu_pos);
        wait_for_button_up();
      } else if (digitalRead(MENU_SELECT_BUTTON) == LOW) {
        if (enter_menu_item(menu_pos) == EXIT_FROM_MENU) {
          break;
        } else {  // remain in menu
          lcd->clear();
          render_menu(menu_pos);
        }
      }
    }
    if (!this->config.backlight)
      lcd->noBacklight();  // turn off the backlight if it's configed off.
    lcd->clear();
  }
};
#endif  // GAS_SENSOR_SENSORMENU_H_
