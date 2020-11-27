// Copyright 2020 Brett M Leedy
/*
 * TestAll.cpp
 */

#include "gtest/gtest.h"

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
