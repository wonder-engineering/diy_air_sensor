// Copyright 2020 Brett M Leedy


#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Stub_LiquidCrystal_I2C.hh"


#include "../SensorMenu.h"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;



//test that:
// init calls EEPROM to get config
// init loads defaults if no EEPROM is available and commits the config
// init stores the LCD pointer
TEST(SensorMenu, init){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);

}

//test that:
// menu_length is the same as the number of menu lines
// checksum covers all settings (set expectation on settings size)
// menu lines are not longer than max width
// there's a case to handle each one of them
TEST(SensorMenu, Menu_Lines){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);


}


// loading to EEPROM works
// loading from EEPROM works
// checksum works
// checksum failures cause loading from defaults
// loading all zeros causes a checksum failure
TEST(SensorMenu, EEPROM_Config){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);


}

// we wait for any one of the buttons to be pressed before moving on
// we time out after a certain number of seconds
TEST(SensorMenu, wait_for_button_up){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}



// we clear the screen
// we call print at least once
// all calls to setCurser are within a sane range
// we return REMAIN_IN_MENU
TEST(SensorMenu, display_sensor_setting){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}

// we call display first
// when menu select button is pressed, we commit the config then wait for button up then return.
// when we press the menu up button we increment our sensor setting, display it, then wait for button up but do not commit.
// when we press the menu up button we decrement our sensor setting, display it, then wait for button up but do not commit.
// we reject bad pointers
TEST(SensorMenu, sensor_settings_callback){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}



// we toggle the backlight setting and commit the config
TEST(SensorMenu, backlight_callback){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}


// we toggle the logging setting and commit the config
TEST(SensorMenu, logging_callback){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}


// we toggle the sensor type setting and commit the config
// after that we call delays that sum up to >3 seconds and call resetFunc
TEST(SensorMenu, sensor_t_callback){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}


// we clear the screen
// setcursor positions are sane
// we call print at least once
// line strings are not too long
TEST(SensorMenu, display_lograte_menu){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}

// we call display first
// when menu select button is pressed, we commit the config then wait for button up then return.
// when we press the menu up button we increment our setting, display it, then wait for button up but do not commit.
// when we press the menu up button we decrement our setting, display it, then wait for button up but do not commit.
// we return remain_in_menu
TEST(SensorMenu, lograte_callback){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}

// we clear the screen
// setcursor positions are sane
// we call print at least once
// line strings are not too long
TEST(SensorMenu, display_sampling_menu){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}

// we call display first
// when menu select button is pressed, we commit the config then wait for button up then return.
// when we press the menu up button we increment our setting, display it, then wait for button up but do not commit.
// when we press the menu up button we decrement our setting, display it, then wait for button up but do not commit.
// we return remain_in_menu
TEST(SensorMenu, sampling_callback){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}

// we clear the screen
// setcursor positions are sane
// we call print at least once
// line strings are not too long
TEST(SensorMenu, display_file_menu){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}

// we call display first
// when menu select button is pressed, we commit the config then wait for button up then return.
// when we press the menu up button we increment our setting, display it, then wait for button up but do not commit.
// when we press the menu up button we decrement our setting, display it, then wait for button up but do not commit.
// we return remain_in_menu
TEST(SensorMenu, file_callback){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}


// we clear the screen
// setcursor positions are sane
// we call print at least once
// line strings are not too long
// todo: this needs to be refactored out
TEST(SensorMenu, display_disp_menu){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}

// we call display first
// when menu select button is pressed, we commit the config then wait for button up then return.
// when we press the menu up button we increment our setting, display it, then wait for button up but do not commit.
// when we press the menu up button we decrement our setting, display it, then wait for button up but do not commit.
// we return remain_in_menu
TEST(SensorMenu, disp_callback){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}

// We render only 3 lines
// We render correct menu at zero
// we render correct menu at last item
// we turn the cursor on
// we render setcursor
TEST(SensorMenu, render_menu){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}


// we have a case for every menu item on the list
// there's a callback associated with every menu item
// we never drop through to default for all good indices
// if we force bad input, we do print a warning
// we only call one callback at a time
TEST(SensorMenu, enter_menu_item){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}



// we turn the backlight on
// if backlight is configed off, we turn it off when we exit
// we render the menu initially
// we render the menu every time we get a button press
// we return if the menu button is pressed and enter_menu returns
// up/down buttons work as expected
TEST(SensorMenu, enter_menu){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);
}
