// Copyright 2020 Brett M Leedy
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/AirSensorDisplay.hh"


using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Le;
using ::testing::AllOf;
using ::testing::An;
using ::testing::Ge;
using ::testing::Mock;
using ::testing::NotNull;
using ::testing::InSequence;
using ::testing::Matcher;
using ::testing::StartsWith;



SensorState fake_sensor_state = {
  .sensor[0].data.value = 0.0,
  .sensor[0].data.avg = 0.0,
  .sensor[0].data.raw = 0,
  .sensor[0].config.shortname = "GAS0",
  .sensor[1].data.value = 10.0,
  .sensor[1].data.avg = 10.0,
  .sensor[1].data.raw = 10,
  .sensor[1].config.shortname = "GAS1",
  .sensor[2].data.value = 100.0,
  .sensor[2].data.avg = 100.0,
  .sensor[2].data.raw = 100,
  .sensor[2].config.shortname = "GAS2",
  .sensor[3].data.value = 1000.0,
  .sensor[3].data.avg = 1000.0,
  .sensor[3].data.raw = 1000,
  .sensor[3].config.shortname = "GAS3",
  .sensor[4].data.value = 888.0,
  .sensor[4].data.avg = 888.0,
  .sensor[4].data.raw = 888,
  .sensor[4].config.shortname = "GAS4",
  .sensor[5].data.value = 0.0,
  .sensor[5].data.avg = 500.0,
  .sensor[5].data.raw = 0,
  .sensor[5].config.shortname = "GAS5",
  .device.file_status = kFileOk,
  .device.num_sensors = 6,
  .device.settings.data.sensor_thresholds[0] = 100,
  .device.settings.data.sensor_thresholds[1] = 100,
  .device.settings.data.sensor_thresholds[2] = 100,
  .device.settings.data.sensor_thresholds[3] = 100,
  .device.settings.data.sensor_thresholds[4] = 100,
  .device.settings.data.sensor_thresholds[5] = 100
  // other settings left as defaults
};


TEST(AirSensorDisplay, display_data) {
  class InstrumentedAirSensorDisplay : public AirSensorDisplay {
   public:
    MOCK_METHOD(void,
                checkForMenuButtons,
                (SensorState * sensor_state),
                (override));
    MOCK_METHOD(void,
                displaySensorColumns,
                (SensorState * sensor_state),
                (override));
    MOCK_METHOD(void,
                displaySensorThresholdWarnings,
                (SensorState * sensor_state),
                (override));
  } display;

  {
    InSequence s;
    // Expect these methods to be called in sequence
    EXPECT_CALL(display, checkForMenuButtons(_));
    EXPECT_CALL(display, displaySensorColumns(_));
    EXPECT_CALL(display, displaySensorThresholdWarnings(_));
  }
  display.display_data(&fake_sensor_state);
}



// Should write the sensor info in the top 3 rows
TEST(AirSensorDisplay, displaySensorColumns) {
  class MockLcd : public LiquidCrystal_I2C {
   public:
    MockLcd(uint8_t lcd_Addr, uint8_t lcd_cols,
            uint8_t lcd_rows) : LiquidCrystal_I2C
              {lcd_Addr, lcd_cols, lcd_rows} {}
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(void, print, (char * toprint));
    MOCK_METHOD(void, print, (const char * toprint));
    MOCK_METHOD(void, print, (uint16_t toprint));
    MOCK_METHOD(void, print, (uint16_t toprint, uint8_t precision));
    MOCK_METHOD(void, setCursor, (uint8_t, uint8_t));
  } mockLcd(1, 3, 4);

  class InstrumentedAirSensorDisplay : public AirSensorDisplay {
   public:
    void replace_lcd(MockLcd * lcd) {this->lcd = lcd;}
    void displaySensorColumns(SensorState * sensor_state) {
      AirSensorDisplay::displaySensorColumns(sensor_state);
    }
  } display;

  display.replace_lcd(&mockLcd);

  // with this model display, we expect columns to be in the
  //   exact same place
  EXPECT_CALL(mockLcd, setCursor(0, AllOf(Ge(0), Le(2)))).Times(3);
  EXPECT_CALL(mockLcd,
              setCursor(LCD_SENSOR_COLUMNS,
              AllOf(Ge(0), Le(2)))).Times(3);

  // Should print 6 labels
  EXPECT_CALL(mockLcd, print(testing::An<char *>())).Times(6);
  // Should print 6 dividers
  EXPECT_CALL(mockLcd, print(":")).Times(6);
  // Should print 6 values
  EXPECT_CALL(mockLcd, print(_, _)).Times(6);

  display.displaySensorColumns(&fake_sensor_state);
}


// Should write warnings on the bottom row
TEST(AirSensorDisplay, displaySensorThresholdWarnings) {
  class MockLcd : public LiquidCrystal_I2C {
   public:
    MockLcd(uint8_t lcd_Addr, uint8_t lcd_cols,
            uint8_t lcd_rows) : LiquidCrystal_I2C
              {lcd_Addr, lcd_cols, lcd_rows} {}
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(void, write, (uint8_t glyph));
    MOCK_METHOD(void, print, (char * toprint));
    MOCK_METHOD(void, print, (const char * toprint));
    MOCK_METHOD(void, print, (uint16_t toprint));
    MOCK_METHOD(void, print, (uint16_t toprint, uint8_t precision));
    MOCK_METHOD(void, setCursor, (uint8_t, uint8_t));
  } mockLcd(1, 3, 4);

  class InstrumentedAirSensorDisplay : public AirSensorDisplay {
   public:
    void replace_lcd(MockLcd * lcd) {this->lcd = lcd;}
    void displaySensorThresholdWarnings(SensorState * sensor_state) {
      AirSensorDisplay::displaySensorThresholdWarnings(sensor_state);
    }
  } display;

  display.replace_lcd(&mockLcd);

  // only set cursor to the last line
  EXPECT_CALL(mockLcd, setCursor(_, 3)).Times(AtLeast(1));

  // should print warning for the 4th sensor, "GAS3"
  EXPECT_CALL(mockLcd, print(Matcher<char*>(StartsWith("GAS3"))));
  EXPECT_CALL(mockLcd, print(" WARNING "));
  EXPECT_CALL(mockLcd, write(byte(SKULL_GLYPH)));
  EXPECT_CALL(mockLcd, write(byte(FILE_OK_GLYPH)));

  display.displaySensorThresholdWarnings(&fake_sensor_state);
}

// should check menu buttons
TEST(AirSensorDisplay, checkForMenuButtons) {
  LiquidCrystal_I2C fake_lcd(1, 1, 1);

  class StubSensorMenu : public SensorMenu {
   public:
    explicit StubSensorMenu(LiquidCrystal_I2C * lcd) : SensorMenu {lcd} {}
    void enter_menu(SensorState * state) {}
  } stubSensorMenu(&fake_lcd);

  class InstrumentedAirSensorDisplay : public AirSensorDisplay {
   public:
    void checkForMenuButtons(SensorState * sensor_state) override {
      AirSensorDisplay::checkForMenuButtons(sensor_state);
    }
    MOCK_METHOD(bool, checkButton, (uint8_t button), (override));
    void replace_sensor_menu(StubSensorMenu * newmenu) {
      this->menu = newmenu;
    }
  } display;

  display.replace_sensor_menu(&stubSensorMenu);

  {
    InSequence s;

    // button held down
    EXPECT_CALL(display, checkButton(_)).Times(
      AtLeast(50)).WillRepeatedly(
        Return(true));

    // button up
    EXPECT_CALL(display, checkButton(_)).Times(
      AtLeast(3)).WillRepeatedly(
        Return(false));
  }


  display.checkForMenuButtons(&fake_sensor_state);
}
