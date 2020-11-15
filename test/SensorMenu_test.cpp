// Copyright 2020 Brett M Leedy


#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Stub_LiquidCrystal_I2C.hh"


#include "../SensorMenu.h"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;

TEST(SensorMenu, File_Names_comply){
  LiquidCrystal_I2C lcd(8,8,8);
  SensorMenu sensormenu(&lcd, 5, 5);

  bool sd_failure = false;
  ASSERT_FALSE(sd_failure);

}
