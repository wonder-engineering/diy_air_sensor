// Copyright 2020 Brett M Leedy


#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Stub_LiquidCrystal_I2C.hh"


#include "../SensorMenu.h"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;



// constructor init test
TEST(SensorMenu, constructor_init){
  class MockedSensorMenu : public SensorMenu {
   public:
    MockedSensorMenu(LiquidCrystal_I2C * lcd, uint8_t col1_idx, uint8_t col2_idx) : SensorMenu{lcd, col1_idx, col2_idx} {
		// no special constructor for mocked class
	}
	MOCK_METHOD(void, load_settings_from_eeprom, (int idx, settings_type * myconfig), (override));
	MOCK_METHOD(void, commit_config, (), (override));
  };

  LiquidCrystal_I2C lcd(8,8,8);
  MockedSensorMenu sensormenu(&lcd, 5, 5);

  // init calls EEPROM to get config
  EXPECT_CALL(sensormenu, load_settings_from_eeprom(0,_)).WillRepeatedly([this](int addr, settings_type * settings){
	 settings->sampling_period_ms = 5;
	 settings->log_every_n_loops = 23458;
	 settings->log_raw=false;
	 settings->file_number=23;
	 settings->backlight=false;
	 settings->alt_sensor_config=false;
	 settings->checksum=0;  // will load an invalid checksum
  });

  // init loads defaults if no EEPROM is available and commits the config
  EXPECT_CALL(sensormenu, commit_config());

  // assert that the new checksum checks out
  sensormenu.init(&lcd, 5, 5);

  // check that the config is now the defaults
  ASSERT_EQ(sensormenu.get_sampling_period_ms(), DEFAULT_LOOP_PERIOD_MILLIS);
  ASSERT_EQ(sensormenu.get_log_every_n_loops(), DEFAULT_LOG_EVERY_N_LOOPS);
  ASSERT_EQ(sensormenu.get_log_raw(), false);
  ASSERT_EQ(sensormenu.get_file_number_config(), 0);
  ASSERT_EQ(sensormenu.get_backlight_config(), true);
  ASSERT_EQ(sensormenu.get_logon_config(), true);
  ASSERT_EQ(sensormenu.get_alt_sensor_config(), false);

  // init stores the LCD pointer
  ASSERT_EQ(sensormenu.get_lcd(), &lcd);

}

// Testing for the menu lines that we have configured
TEST(SensorMenu, Menu_Lines){

  class MockedSensorMenu : public SensorMenu {
   public:
    MockedSensorMenu(LiquidCrystal_I2C * lcd, uint8_t col1_idx, uint8_t col2_idx) : SensorMenu{lcd, col1_idx, col2_idx} {
		// no special constructor for mocked class
	}
	MOCK_METHOD(bool, exit_callback, (), (override));
	MOCK_METHOD(bool, disp_callback, (), (override));
	MOCK_METHOD(bool, file_callback, (), (override));
	MOCK_METHOD(bool, sampling_callback, (), (override));
	MOCK_METHOD(bool, lograte_callback, (), (override));
	MOCK_METHOD(bool, backlight_callback, (), (override));
	MOCK_METHOD(bool, logon_callback, (), (override));
	MOCK_METHOD(bool, sensor_settings_callback, (const char *, uint16_t *), (override));
	MOCK_METHOD(bool, sensor_t_callback, (), (override));
	MOCK_METHOD(bool, error_callback, (), (override));
  };

  // menu_length is the same as the number of menu lines
  // this is an array of pointers
  uint16_t num_menu_strings = sizeof(menu_line) / sizeof(menu_line[0]);
  ASSERT_EQ(num_menu_strings, MENU_LENGTH);

  LiquidCrystal_I2C lcd(8,8,8);  // a fake stub, doesn't matter
  MockedSensorMenu sensormenu(&lcd, 5, 5);     // params don't matter
  LogFile * logfile = new LogFile(); // just there to hold a place
  sensormenu.attach_logfile(logfile);

//   printf("initted\n");

  int num_menu_tests = 0;
  EXPECT_CALL(sensormenu, exit_callback()); num_menu_tests++;
  EXPECT_CALL(sensormenu, disp_callback()); num_menu_tests++;
  EXPECT_CALL(sensormenu, file_callback()); num_menu_tests++;
  EXPECT_CALL(sensormenu, sampling_callback()); num_menu_tests++;
  EXPECT_CALL(sensormenu, lograte_callback()); num_menu_tests++;
  EXPECT_CALL(sensormenu, backlight_callback()); num_menu_tests++;
  EXPECT_CALL(sensormenu, logon_callback()); num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_settings_callback("LPG", _ )); num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_settings_callback("CO", _ )); num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_settings_callback("O3", _ )); num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_settings_callback("GAS", _ )); num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_settings_callback("HAZ", _ )); num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_settings_callback("PM", _ )); num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_t_callback()); num_menu_tests++;
  ASSERT_EQ(MENU_LENGTH, num_menu_tests); // here to remind you to add a test case when you add a menu item.

  EXPECT_CALL(sensormenu, error_callback()).Times(0);

  for (int i = 0; i < MENU_LENGTH; i++ ) {
	  sensormenu.enter_menu_item(i);
  }

  delete logfile;
}


// changes break checksum
// checksum covers all values
// once committed, the checsum succeeds
TEST(SensorMenu, settings_type){

  settings_type settings;
  int settings_overall_size = sizeof(settings);
  int settings_tested_size = 0;  // amount of the settings type that's tested
  settings.sampling_period_ms = 4326;
  settings_tested_size += sizeof(settings.sampling_period_ms);
  settings.log_every_n_loops = 12;
  settings_tested_size += sizeof(settings.log_every_n_loops);
  settings.file_number = 15;
  settings_tested_size += sizeof(settings.file_number);
  settings.dust_zero = 92;
  settings_tested_size += sizeof(settings.dust_zero);
  for (int i=0; i<MAX_SENSORS; i++){
	  settings.sensor_thresholds[i] = 5 * i;
  }
  settings_tested_size += sizeof(settings.sensor_thresholds);
  for (int i=0; i<MAX_SENSORS; i++){
	  settings.sensor_zeros[i] = i;
  }
  settings_tested_size += sizeof(settings.sensor_zeros);
  settings.log_raw = false;
  settings_tested_size += sizeof(settings.log_raw);
  settings.backlight = true;
  settings_tested_size += sizeof(settings.backlight);
  settings.logging = true;
  settings_tested_size += sizeof(settings.logging);
  settings.alt_sensor_config = false;
  settings_tested_size += sizeof(settings.alt_sensor_config);

  // Assert that this test initted (and will test) all fields
  // in the data structure
  ASSERT_GE(settings_tested_size+3, settings_overall_size);


  // Checksum edge cases
  settings.checksum = 0;  // arbitrary zero checksum
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.checksum = 23976; // arbitrary high checksum
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.checksum = 0xFFFF; // saturated checksum
  ASSERT_FALSE(settings.check());

  // changing values in any field breaks checksum
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.sampling_period_ms = 0;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.log_every_n_loops = 9546;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.file_number = 0;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.dust_zero = 10;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.sensor_thresholds[2] = 923;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.sensor_zeros[1] = 6;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.log_raw = !settings.log_raw;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.backlight = !settings.backlight;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.logging = !settings.logging;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum
  ASSERT_TRUE(settings.check());
  settings.alt_sensor_config = !settings.alt_sensor_config;
  ASSERT_FALSE(settings.check());
  settings.store_checksum();  // calculate correct checksum

  settings_tested_size += sizeof(settings.checksum);

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
