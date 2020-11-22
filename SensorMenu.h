// Copyright 2020 Brett M. Leedy
// todo: move code to cpp for reusabiltiy.
// todo: This classcurrently takes in pointers to  other things it has no business
//   changing and calls methods to things.  In an ideal world, this class will only
//   be responsible for updating a config, that other things will then consume as
//   the user-input state.  This is a very high priority.


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

#include "Arduino.h"
#include "SensorState.hh"
#include <stdint.h>
#include "LiquidCrystal_I2C.h"

// Menu Buttons
#define MENU_SELECT_BUTTON 8
#define MENU_UP_BUTTON     6
#define MENU_DN_BUTTON     5


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




class SensorMenu {

 LiquidCrystal_I2C * lcd;

 public:
  SensorMenu(LiquidCrystal_I2C * lcd) {
    init(lcd);
  }

  void init(LiquidCrystal_I2C * lcd) {
    this->lcd = lcd;
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


  virtual bool sensor_settings_callback(const char * name,
                                        uint16_t * setting,
                                        SensorSettings * settings) {
    Serial.print(F("Entered settings callback for ")); Serial.println(name);

    this->display_sensor_setting(name, setting);

    wait_for_button_up();
    while (true) {
      if (digitalRead(MENU_SELECT_BUTTON) == LOW) {
        settings->commit();  // write to EEPROM before exiting
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

  virtual bool backlight_callback(SensorSettings * settings) {
    settings->data.backlight = !settings->data.backlight;
    settings->commit();
    return EXIT_FROM_MENU;  // exit back out to main display
  }

  virtual bool logon_callback(SensorSettings * settings) {
    settings->data.logging = !settings->data.logging;
    settings->commit();
    return EXIT_FROM_MENU;  // exit back out to main display
  }

  virtual bool sensor_t_callback(SensorSettings * settings) {
    if (settings->data.alt_sensor_config)
      settings->data.alt_sensor_config = false;
    else
      settings->data.alt_sensor_config = true;
    settings->commit();
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

  void display_lograte_menu(SensorSettings * settings) {
    Serial.println(F("Entered log rate Callback"));
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("Logging every N "));
    lcd->setCursor(0, 1);
    lcd->print(F("samples:  "));
    lcd->print(settings->data.log_every_n_loops);
    lcd->setCursor(0, 2);
    lcd->print(F("Use arrows to set."));
    lcd->setCursor(0, 3);
    lcd->print(F("Blue to exit."));
  }

  virtual bool lograte_callback(SensorSettings * settings) {
      display_lograte_menu(settings);
      wait_for_button_up();
      while (true) {
        if (digitalRead(MENU_SELECT_BUTTON) == LOW) {
          settings->commit();  // write to EEPROM before exiting
          wait_for_button_up();
          return REMAIN_IN_MENU;
        } else if (digitalRead(MENU_UP_BUTTON) == LOW) {
          settings->data.log_every_n_loops++;
          this->display_lograte_menu(settings);
          wait_for_button_up();
        } else if (digitalRead(MENU_DN_BUTTON) == LOW) {
          if ( settings->data.log_every_n_loops > 1)
            settings->data.log_every_n_loops--;
          this->display_lograte_menu(settings);
          wait_for_button_up();
        }
      }
  }


  void display_sampling_menu(SensorSettings * settings) {
    Serial.println(F("Entered sample rate Callback"));
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("Current sample rate:"));
    lcd->setCursor(0, 1);
    lcd->print(F("Period, ms: "));
    lcd->print(settings->data.sampling_period_ms);
    lcd->setCursor(0, 2);
    lcd->print(F("Use arrows to set."));
    lcd->setCursor(0, 3);
    lcd->print(F("Blue to exit."));
  }

  virtual bool sampling_callback(SensorSettings * settings) {
      display_sampling_menu(settings);
      wait_for_button_up();
      while (true) {
        if (digitalRead(MENU_SELECT_BUTTON) == LOW) {
          settings->commit();  // write to EEPROM before exiting
          wait_for_button_up();
          return REMAIN_IN_MENU;
        } else if (digitalRead(MENU_UP_BUTTON) == LOW) {
          settings->data.sampling_period_ms += 500;
          this->display_sampling_menu(settings);
          wait_for_button_up();
        } else if (digitalRead(MENU_DN_BUTTON) == LOW) {
          if (settings->data.sampling_period_ms >= 1000)
            settings->data.sampling_period_ms -= 500;
          this->display_sampling_menu(settings);
          wait_for_button_up();
        }
      }
  }

  void display_file_menu(SensorState * state) {
    Serial.println(F("Entered file Callback"));
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("Current File: "));
    lcd->setCursor(0, 1);
    lcd->print(F("   "));
    lcd->print(state->device.current_file_name);
    lcd->setCursor(0, 2);
    lcd->print(F("Use arrows to set."));
    lcd->setCursor(0, 3);
    lcd->print(F("Blue to exit."));
  }

  virtual bool file_callback(SensorState * state) {
    display_file_menu(state);
    wait_for_button_up();
    while (true) {
      if (digitalRead(MENU_SELECT_BUTTON) == LOW) {
        state->device.settings.commit();  // write to EEPROM before exiting
        wait_for_button_up();
        return REMAIN_IN_MENU;
      } else if (digitalRead(MENU_UP_BUTTON) == LOW ||
                  digitalRead(MENU_DN_BUTTON) == LOW) {
        // todo: add this functionality
        display_file_menu(state);
        wait_for_button_up();
      }
    }
  }

  bool display_raw = false;
  virtual bool disp_callback(SensorSettings * settings) {
    Serial.println(F("Entered Display Callback"));
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("Press up or down to "));
    lcd->setCursor(0, 1);
    lcd->print(F("   set display type."));
    lcd->setCursor(0, 2);
    lcd->print(F("Blue button to exit."));
    lcd->setCursor(7, 3);
    if (settings->data.display_raw)
      lcd->print(F("Raw    "));
    else
      lcd->print(F("Average"));

    while (true) {
      if (digitalRead(MENU_SELECT_BUTTON) == LOW) {
        // if (dust    != NULL) {dust->set_display_raw(display_raw);}  //fixme
        // if (sensors != NULL) {sensors->set_display_raw(display_raw);}  //fixme
        wait_for_button_up();
        settings->commit(); // commit before exiting
        return REMAIN_IN_MENU;
      } else if (digitalRead(MENU_UP_BUTTON) == LOW ||
                  digitalRead(MENU_DN_BUTTON) == LOW) {
        settings->data.display_raw = !settings->data.display_raw;
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
      lcd->setCursor(0, d_row-line);
      lcd->print(buffer);
      Serial.print("render line "); Serial.println(d_row);
    }
    lcd->setCursor(0, 0);
    lcd->cursor();
  }

  bool enter_menu_item(uint8_t id, SensorState * state) {
    Serial.print(F("entering menu item ")); Serial.println(id);
    bool rv = REMAIN_IN_MENU;
    switch (id) {
      case 0:
        rv = exit_callback(); break;
      case 1:
        rv = disp_callback(&state->device.settings); break;
      case 2:
        rv = file_callback(state); break;
      case 3:
        rv = sampling_callback(&state->device.settings); break;
      case 4:
        rv = lograte_callback(&state->device.settings); break;
      case 5:
        rv = backlight_callback(&state->device.settings); break;
      case 6:
        rv = logon_callback(&state->device.settings); break;
      case 7:
        rv = sensor_settings_callback("LPG",
              &state->device.settings.data.sensor_thresholds[0],
              &state->device.settings);
        break;  //  7: LPG Gas Sensor
      case 8:
        rv = sensor_settings_callback("CO",
              &state->device.settings.data.sensor_thresholds[1],
              &state->device.settings);
        break;  //  8: Carbon Monoxide Sensor
      case 9:
        rv = sensor_settings_callback("O3",
              &state->device.settings.data.sensor_thresholds[2],
              &state->device.settings);
        break;  //  9: Ozone Sensor
      case 10:
        rv = sensor_settings_callback("GAS",
              &state->device.settings.data.sensor_thresholds[3],
              &state->device.settings);
        break;  // 10: Gas Sensor
      case 11:
        rv = sensor_settings_callback("HAZ",
              &state->device.settings.data.sensor_thresholds[4],
              &state->device.settings);
        break;  // 11: Hazardous Gas Sensor
      case 12:
        rv = sensor_settings_callback("PM",
              &state->device.settings.data.sensor_thresholds[5],
              &state->device.settings);
        break;  // 12: Dust sensor adjustment
      case 13:
        rv = sensor_t_callback(&state->device.settings);
        break;  // 13: toggle the set of sensors I'll use on next restart
      default:
        Serial.println(F("No function exists for this menu item"));
        error_callback();
    }
    return rv;
  }

  virtual bool error_callback(){ return false;}

  void enter_menu(SensorState * state) {
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
        if (enter_menu_item(menu_pos, state) == EXIT_FROM_MENU) {
          break;
        } else {  // remain in menu
          lcd->clear();
          render_menu(menu_pos);
        }
      }
    }
    lcd->clear();
  }
};
#endif  // GAS_SENSOR_SENSORMENU_H_
