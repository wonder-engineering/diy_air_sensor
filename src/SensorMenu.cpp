// Copyright 2020 Brett M. Leedy

#include "SensorMenu.hh"
#define MENU_LINE_BUFFER_LEN  25

  SensorMenu::SensorMenu(LiquidCrystal_I2C * lcd) {
    init(lcd);
  }

  void SensorMenu::init(LiquidCrystal_I2C * lcd) {
    this->lcd = lcd;
  }

  void SensorMenu::wait_for_button_up() {
    while (digitalRead(MENU_SELECT_BUTTON) == LOW ||
           digitalRead(MENU_UP_BUTTON) == LOW ||
           digitalRead(MENU_DN_BUTTON) == LOW) {
            // do nothing until all buttons released for at least one cycle
            delay(1);
          }
  }

  bool SensorMenu::display_sensor_setting(const char * name,
                                          uint16_t * setting) {
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


  bool SensorMenu::sensor_settings_callback(const char * name,
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

  bool SensorMenu::exit_callback() {
    return EXIT_FROM_MENU;
  }

  bool SensorMenu::backlight_callback(SensorSettings * settings) {
    settings->data.backlight = !settings->data.backlight;
    settings->commit();
    return EXIT_FROM_MENU;  // exit back out to main display
  }

  bool SensorMenu::logon_callback(SensorSettings * settings) {
    settings->data.logging = !settings->data.logging;
    settings->commit();
    return EXIT_FROM_MENU;  // exit back out to main display
  }

  bool SensorMenu::sensor_t_callback(SensorSettings * settings) {
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

  void SensorMenu::restart() {
    void(* resetFunc) (void) = 0;
    resetFunc();
  }

  void SensorMenu::display_lograte_menu(SensorSettings * settings) {
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

  bool SensorMenu::lograte_callback(SensorSettings * settings) {
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

void SensorMenu::display_sampling_menu(SensorSettings * settings) {
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

bool SensorMenu::sampling_callback(SensorSettings * settings) {
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

void SensorMenu::display_file_menu(SensorState * state) {
    Serial.println(F("Entered file Callback"));
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("Current File: "));
    lcd->setCursor(0, 1);
    lcd->print(F("   "));
    // lcd->print(state->device.current_file_name);
    // todo: use file index instead
    lcd->setCursor(0, 2);
    lcd->print(F("Use arrows to set."));
    lcd->setCursor(0, 3);
    lcd->print(F("Blue to exit."));
}

bool SensorMenu::file_callback(SensorState * state) {
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


bool SensorMenu::disp_callback(SensorSettings * settings) {
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
        wait_for_button_up();
        settings->commit();  // commit before exiting
        return REMAIN_IN_MENU;
        } else if (digitalRead(MENU_UP_BUTTON) == LOW ||
                    digitalRead(MENU_DN_BUTTON) == LOW) {
        settings->data.display_raw = !settings->data.display_raw;
        lcd->setCursor(7, 3);
        if (settings->data.display_raw)
            lcd->print(F("Raw    "));
        else
            lcd->print(F("Average"));
        wait_for_button_up();  // wait a  moment to check again
        }
    }
}


// Render the menu, starting with a specific line
void SensorMenu::render_menu(uint8_t line) {
lcd->clear();
for (int d_row=line; d_row < line + 4 && d_row < MENU_LENGTH; d_row++) {
    char buffer[MENU_LINE_BUFFER_LEN];  // make sure this is large enough
    strcpy_P(buffer,
    reinterpret_cast<char *>(pgm_read_word(&(menu_line[d_row]))));
    lcd->setCursor(0, d_row-line);
    lcd->print(buffer);
    Serial.print("render line "); Serial.println(d_row);
}
lcd->setCursor(0, 0);
lcd->cursor();
}

bool SensorMenu::enter_menu_item(uint8_t id, SensorState * state) {
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

bool SensorMenu::error_callback() { return false;}

void SensorMenu::enter_menu(SensorState * state) {
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
