// Copyright 2020 Brett M Leedy


#include "gtest/gtest.h"
#include "gmock/gmock.h"


#include "../AnalogSensor.h"
#include "../MQSensor.hh"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;
using ::testing::A;
using ::testing::Gt;
using ::testing::Lt;


#define SANE_TEST_ID       19
#define SANE_GAIN          10.0
#define SANE_LCD_HEIGHT     4
#define SANE_ACCUM_RATE   0.1
#define SANE_ZERO_ADJUST   100
#define LCD_FIRST_ROW       0


TEST(AnalogSensor, add_sensor) {

	AnalogSensor analogsensor;

	char short_name_buffer[SENSOR_SHORT_NAME_LEN];
	// Successfully add the max number of sensors.
	for (int sensor_number = 0; sensor_number < MAX_NUM_SENSORS; sensor_number++ ) {

		ASSERT_LE(snprintf(short_name_buffer, sizeof(short_name_buffer), "S_%d", sensor_number),
							SENSOR_SHORT_NAME_LEN);  // make sure test name meets short name requirement
		ASSERT_TRUE(analogsensor.add_sensor(new MQSensor(short_name_buffer, SANE_ACCUM_RATE,
								sensor_number, SANE_ZERO_ADJUST, SANE_GAIN)));
		ASSERT_EQ(analogsensor.get_num_sensors(), sensor_number+1);
		char compare_name_buffer[SENSOR_SHORT_NAME_LEN];
		analogsensor.get_short_name(sensor_number, compare_name_buffer, sizeof(short_name_buffer));
		ASSERT_STREQ(short_name_buffer, compare_name_buffer);

		// State should start zeroed always
		ASSERT_EQ(analogsensor.get_sensor_avg(analogsensor.get_num_sensors()-1), SENSOR_DEFAULT_AVG_VALUE);
		ASSERT_EQ(analogsensor.get_sensor_raw(analogsensor.get_num_sensors()-1), SENSOR_DEFAULT_LAST_VALUE);

	}

	// Confirm test filled up the sensors
    ASSERT_EQ(analogsensor.get_num_sensors(), MAX_NUM_SENSORS);

    // I cannot add more sensors after the max:
	ASSERT_LE(snprintf(short_name_buffer, sizeof(short_name_buffer), "S_%d", SANE_TEST_ID),
					SENSOR_SHORT_NAME_LEN);  // make sure test name meets short name requirement
	ASSERT_FALSE(analogsensor.add_sensor(new MQSensor(short_name_buffer, SANE_ACCUM_RATE,
								SANE_TEST_ID, SANE_ZERO_ADJUST, SANE_GAIN)));
	ASSERT_EQ(analogsensor.get_num_sensors(), MAX_NUM_SENSORS);

}
