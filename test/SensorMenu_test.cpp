// Copyright 2020 Brett M Leedy

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Stub_LiquidCrystal_I2C.hh"


#include "../src/SensorMenu.hh"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;
using ::testing::A;
using ::testing::Gt;
using ::testing::Lt;


#define SANE_LCD_WIDTH          20
#define SANE_LCD_COL1            0
#define SANE_LCD_COL2            SANE_LCD_WIDTH/2
#define SANE_LCD_HEIGHT          4
#define SANE_SAMPLING_PERIOD_MS  5
#define SANE_LOG_EVERY_N_LOOPS  15
#define SANE_FILE_NUMBER        23
#define SANE_LCD_ADDR           10
#define SANE_SETTING_NAME    "TST"
#define SANE_SETTING_VALUE    2348


// Testing for the menu lines that we have configured
TEST(SensorMenu, Menu_Lines) {
  class MockedSensorMenu : public SensorMenu {
   public:
    explicit MockedSensorMenu(LiquidCrystal_I2C * lcd) : SensorMenu{lcd} {
    // no special constructor for mocked class
  }
  MOCK_METHOD(bool, exit_callback, (), (override));
  MOCK_METHOD(bool, disp_callback, (SensorSettings *), (override));
  MOCK_METHOD(bool, file_callback, (SensorState *), (override));
  MOCK_METHOD(bool, sampling_callback, (SensorSettings *), (override));
  MOCK_METHOD(bool, lograte_callback, (SensorSettings *), (override));
  MOCK_METHOD(bool, backlight_callback, (SensorSettings *), (override));
  MOCK_METHOD(bool, logon_callback, (SensorSettings *), (override));
  MOCK_METHOD(bool, sensor_settings_callback, (const char *, uint16_t *,
                                          SensorSettings *), (override));
  MOCK_METHOD(bool, sensor_t_callback, (SensorSettings *), (override));
  MOCK_METHOD(bool, error_callback, (), (override));

  bool enter_menu_item(uint8_t id, SensorState * state) {
    return SensorMenu::enter_menu_item(id, state);
  }
  };

  // menu_length is the same as the number of menu lines
  // this is an array of pointers
  uint16_t num_menu_strings = sizeof(menu_line) / sizeof(menu_line[0]);
  ASSERT_EQ(num_menu_strings, MENU_LENGTH);
  // a fake stub, doesn't matter
  LiquidCrystal_I2C lcd(SANE_LCD_ADDR, SANE_LCD_WIDTH, SANE_LCD_HEIGHT);
  MockedSensorMenu sensormenu(&lcd);     // params don't matter


  int num_menu_tests = 0;
  EXPECT_CALL(sensormenu, exit_callback());
  num_menu_tests++;
  EXPECT_CALL(sensormenu, disp_callback(_));
  num_menu_tests++;
  EXPECT_CALL(sensormenu, file_callback(_));
  num_menu_tests++;
  EXPECT_CALL(sensormenu, sampling_callback(_));
  num_menu_tests++;
  EXPECT_CALL(sensormenu, lograte_callback(_));
  num_menu_tests++;
  EXPECT_CALL(sensormenu, backlight_callback(_));
  num_menu_tests++;
  EXPECT_CALL(sensormenu, logon_callback(_));
  num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_settings_callback("LPG", _, _));
  num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_settings_callback("CO", _, _));
  num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_settings_callback("O3", _, _));
  num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_settings_callback("GAS", _, _));
  num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_settings_callback("HAZ", _, _));
  num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_settings_callback("PM", _, _));
  num_menu_tests++;
  EXPECT_CALL(sensormenu, sensor_t_callback(_)); num_menu_tests++;
  // here to remind you to add a test case when you add a menu item.
  ASSERT_EQ(MENU_LENGTH, num_menu_tests);

  EXPECT_CALL(sensormenu, error_callback()).Times(0);

  SensorState state = SensorState();

  for (int i = 0; i < MENU_LENGTH; i++) {
    sensormenu.enter_menu_item(i, &state);
  }
}
