// Copyright 2020 Brett M Leedy

/*
  The point of this file is to make assertions about the functionality
  of the stubs, fakes, and mocks built into the Stub_* files.

  If these files to change, the validity of our tests could be in question.
*/
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Stub_Arduino.h"
// #include "Stub_EEPROM.hh"
#include "Stub_LiquidCrystal_I2C.hh"
// #include "Stub_SD.h"
#include "Stub_Serial.h"
#include "File_mock.h"

// SD interface mock
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;
using ::testing::NotNull;

#define FAKE_UINT8 1
#define FAKE_UINT16 500
#define FAKE_UINT32 600000
#define FAKE_INT32 -600000
#define FAKE_STRING "hello"
#define FAKE_FLOAT 1234098.2345908173
#define FAKE_CHAR 'a'

TEST(LogFileWriter, stub_arduino) {
  const char * array_of_strings[] = {FAKE_STRING, FAKE_STRING};

  // voids that should do nothing and not crash
  digitalWrite(FAKE_UINT8, FAKE_UINT8);
  delayMicroseconds(FAKE_UINT32);
  delay(FAKE_UINT32);

  // Expect ints to always return same value.
  ASSERT_EQ(0, digitalRead(FAKE_UINT8));
  ASSERT_EQ(0, analogRead(FAKE_UINT8));
  ASSERT_EQ(0, millis());
  ASSERT_EQ(0, pgm_read_word(array_of_strings));
}


TEST(LogFileWriter, stub_lcd) {
  uint8_t chars[] = {FAKE_UINT8, FAKE_UINT8};
  char string[] = FAKE_STRING;
  const char constString[] = FAKE_STRING;

  // inits that do nothing should not crash
  LiquidCrystal_I2C lcd(FAKE_UINT8, FAKE_UINT8, FAKE_UINT8);
  lcd.begin(FAKE_UINT8, FAKE_UINT8, FAKE_UINT8);

  // voids that should do nothing and not crash
  lcd.clear();
  lcd.home();
  lcd.noDisplay();
  lcd.display();
  lcd.noBlink();
  lcd.blink();
  lcd.noCursor();
  lcd.cursor();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayRight();
  lcd.printLeft();
  lcd.printRight();
  lcd.leftToRight();
  lcd.rightToLeft();
  lcd.shiftIncrement();
  lcd.shiftDecrement();
  lcd.noBacklight();
  lcd.backlight();
  lcd.autoscroll();
  lcd.noAutoscroll();
  lcd.createChar(FAKE_UINT8, chars);
  lcd.print(string);
  lcd.print(constString);
  lcd.print(FAKE_UINT16);
  lcd.print(FAKE_UINT16, FAKE_UINT8);
  lcd.write(FAKE_UINT8);
  lcd.command(FAKE_UINT8);
  lcd.init();
  lcd.oled_init();
  lcd.blink_on();
  lcd.blink_off();
  lcd.cursor_on();
  lcd.cursor_off();
  lcd.setBacklight(FAKE_UINT8);
  lcd.load_custom_character(FAKE_UINT8, chars);
  lcd.printstr(FAKE_STRING);
}



TEST(LogFileWriter, stub_serial) {
  // void should do nothing and not crash
  Serial.setPrintToCout(true);
  ASSERT_EQ(0, Serial.print(FAKE_CHAR));
  ASSERT_EQ(0, Serial.print((unsigned char)FAKE_CHAR));
  ASSERT_EQ(0, Serial.print(FAKE_UINT8, DEC));
  ASSERT_EQ(0, Serial.print(FAKE_INT32, DEC));
  ASSERT_EQ(0, Serial.print(FAKE_UINT32, DEC));
  ASSERT_EQ(0, Serial.print((long)FAKE_UINT32, DEC));  // NOLINT
  ASSERT_EQ(0, Serial.print((unsigned long)FAKE_UINT32, DEC));  // NOLINT
  ASSERT_EQ(0, Serial.print(FAKE_FLOAT));

  ASSERT_EQ(0, Serial.println(FAKE_CHAR));
  ASSERT_EQ(0, Serial.println((unsigned char)FAKE_CHAR));
  ASSERT_EQ(0, Serial.println(FAKE_UINT8, DEC));
  ASSERT_EQ(0, Serial.println(FAKE_INT32, DEC));
  ASSERT_EQ(0, Serial.println(FAKE_UINT32, DEC));
  ASSERT_EQ(0, Serial.println((long)FAKE_UINT32, DEC));  // NOLINT
  ASSERT_EQ(0, Serial.println((unsigned long)FAKE_UINT32, DEC));  // NOLINT
  ASSERT_EQ(0, Serial.println(FAKE_FLOAT));
  ASSERT_EQ(0, Serial.println());

  ASSERT_EQ(0, Serial.write(FAKE_UINT8));
  ASSERT_EQ(0, Serial.write(FAKE_STRING));

  ASSERT_EQ(0, Serial.available());
  ASSERT_EQ(0, Serial.read());

  Serial.flush();
}

TEST(LogFileWriter, file_mock) {
  File testFile;

  ASSERT_EQ(NULL, testFile.name());
  ASSERT_EQ(0, testFile.open());
  testFile.write(FAKE_UINT8);
}
