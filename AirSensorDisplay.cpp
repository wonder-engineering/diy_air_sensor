// Copyright 2020 Brett M. Leedy

#include "AirSensorDisplay.hh"
#include "custom_lcd_glyphs.h"

void AirSensorDisplay::init_lcd() {
  lcd = new LiquidCrystal_I2C(0x27,LCD_NUM_COLUMNS,LCD_NUM_ROWS);
  lcd->init();
  lcd->clear();
  lcd->createChar(FILE_OK_GLYPH,  file_ok_glyph);
  lcd->createChar(FILE_BAD_GLYPH, file_bad_glyph);
  lcd->createChar(SKULL_GLYPH,    skull_glyph);
  lcd->setCursor(0,0);
  lcd->backlight(); // default to backlight on
}

AirSensorDisplay::AirSensorDisplay() {
  init_lcd();
  Serial.println(F("Init SensorMenu:"));
  this->menu = new SensorMenu(lcd);
}

void AirSensorDisplay::display_data(SensorState * sensor_state) {
  // Clear the LCD to ensure clean slate
  lcd->clear();

  // check whether I need to enter the menu
  if(digitalRead(MENU_SELECT_BUTTON) == LOW ||
     digitalRead(MENU_UP_BUTTON) == LOW ||
     digitalRead(MENU_DN_BUTTON) == LOW){

    Serial.println(F("Enter Menu"));
    // wait for button to be released
    while(digitalRead(MENU_SELECT_BUTTON) == LOW ||
          digitalRead(MENU_UP_BUTTON) == LOW ||
          digitalRead(MENU_DN_BUTTON) == LOW) {
      delay(10);
    }
    menu->enter_menu(sensor_state);
  }

  // print sensor states, rotating them if there are more sensors than
  //   space to display them.
  // todo: support rotating rows if not enough to display all at once
  // Reserve the last row for other display items
  uint8_t row = 0, column = 0;
  for(uint8_t sensor_number = 0;
      sensor_number < sensor_state->device.num_sensors;
      sensor_number++) {
      lcd->setCursor(column,row);
      // print sensor info
      lcd->print(sensor_state->sensor[sensor_number].config.shortname);
      lcd->print(":");
      lcd->print(sensor_state->sensor[sensor_number].data.value, DISPLAY_DATA_PRECISION);

      // move to the next valid cursor location, or stop
      //  (space needed to add a column)   (space available)
      if( (column + LCD_SENSOR_COLUMNS) < (LCD_NUM_COLUMNS) ) {
        column += LCD_SENSOR_COLUMNS;
      } else {  // no column space left, try new row
        if(row < (LCD_NUM_ROWS-1)) { // we have another row available
          row++;        // next row
          column = 0;   // first column
        } else {
          break;  // no space left
        }
      }
  }  // for (sensor_number)

  // print a warning on the last line if a threshold is exceeded
  // Display sensor threshod warnings
  for(uint8_t sensor_number = 0;
      sensor_number < sensor_state->device.num_sensors;
      sensor_number++) {
    if(sensor_state->sensor[sensor_number].data.value >
        sensor_state->device.settings.data.sensor_thresholds[sensor_number] ) {
      lcd->setCursor(0, (LCD_NUM_ROWS-1));
      char cbuffer[SENSOR_SHORT_NAME_LEN];
      lcd->print(sensor_state->sensor[sensor_number].config.shortname);
      lcd->print(F(" WARNING "));
      lcd->write(byte(SKULL_GLYPH));
      break;  // only have room to print one.
    }
  }  // for (sensor_id)

  // Print glyph overlay on right side of last line for file status
  lcd->setCursor((LCD_NUM_COLUMNS-1), (LCD_NUM_ROWS-1));
  if(sensor_state->device.file_status == kFileFail){
    lcd->write(byte(FILE_BAD_GLYPH));  // Dead File
  } else {
    lcd->write(byte(FILE_OK_GLYPH));  // OK File
  }
}
