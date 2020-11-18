// Copyright 2020 Brett M Leedy


#include "gtest/gtest.h"
#include "gmock/gmock.h"


#include "../AnalogSensor.h"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;
using ::testing::A;
using ::testing::Gt;
using ::testing::Lt;


#define SANE_LCD_ADDR      19
#define SANE_LCD_WIDTH     20
#define SANE_LCD_HEIGHT     4
#define SANE_ACCUM_RATE   0.1
#define LCD_FIRST_COLUMN    0
#define LCD_FIRST_ROW       0


TEST(AnalogSensor, add_sensor){

	LiquidCrystal_I2C lcd(SANE_LCD_ADDR, SANE_LCD_WIDTH, SANE_LCD_HEIGHT );
	AnalogSensor analogsensor(&lcd);

	char short_name_buffer[SHORT_NAME_LEN*2]; // allocate double-sized buffer for test
	// Successfully add the max number of sensors.
	for (int sensor_number = 0; sensor_number < MAX_NUM_SENSORS; sensor_number++ ) {

		ASSERT_LE(snprintf(short_name_buffer, sizeof(short_name_buffer), "S_%d", sensor_number),
							SHORT_NAME_LEN);  // make sure test name meets short name requirement
		ASSERT_TRUE(analogsensor.add_sensor(short_name_buffer,
								(sensor_number / SANE_LCD_HEIGHT) * SANE_LCD_WIDTH/2,
								sensor_number % SANE_LCD_HEIGHT,
								sensor_number,
								SANE_ACCUM_RATE));
		sensor_config commited_config = analogsensor.get_sensor_config(analogsensor.get_num_sensors()-1);
		ASSERT_EQ(commited_config.analog_pin, sensor_number);
		ASSERT_GT(commited_config.accum_rate, 0.0);  // check that something is set
		ASSERT_EQ(commited_config.zero_adjust, 0);
		ASSERT_STREQ(commited_config.short_name, short_name_buffer);

		// State should start zeroed always
		ASSERT_EQ(analogsensor.get_sensor_avg(analogsensor.get_num_sensors()-1), 0.0);
		ASSERT_EQ(analogsensor.get_sensor_state(analogsensor.get_num_sensors()-1).last_value, 0);

	}

	// Confirm test filled up the sensors
    ASSERT_EQ(analogsensor.get_num_sensors(), MAX_NUM_SENSORS);

    // I cannot add more sensors after the max:
	ASSERT_LE(snprintf(short_name_buffer, sizeof(short_name_buffer), "S_%d", MAX_NUM_SENSORS),
					SHORT_NAME_LEN);  // make sure test name meets short name requirement
	ASSERT_FALSE(analogsensor.add_sensor(short_name_buffer,
							LCD_FIRST_COLUMN,
							LCD_FIRST_ROW,
							MAX_NUM_SENSORS,
							SANE_ACCUM_RATE));
	ASSERT_EQ(analogsensor.get_num_sensors(), MAX_NUM_SENSORS);

}
